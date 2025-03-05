/*
MIT License

Copyright (c) 2025 [Peter Robinson]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * @file engine.cpp
 * @brief Implementation of the Engine class.
 */

#include "pl_search/engine.hpp"
#include "pl_search/clist.hpp"
#include "pl_search/patom.hpp"
#include "pl_search/pfloat.hpp"
#include "pl_search/pint.hpp"
#include "pl_search/pred.hpp"
#include "pl_search/typedefs.hpp"

#include <fstream>
#include <iostream>

using namespace std;

namespace pl_search {

/**
 * @brief Trails a variable.
 * @param v The variable to trail.
 */
void Engine::trail(PVarPtr v) {
  trail_entry *entry = new trail_entry();
  entry->var = v;
  entry->value = v->value;
  trail_stack.push(entry);
}

/**
 * @brief Performs backtracking.
 */
void Engine::backtrack() {
  int old_top = env_stack.top()->trail_index;

  while (trail_stack.size() > old_top) {
    trail_entry *entry = trail_stack.top();
    entry->var->reset(entry->value);
    trail_stack.pop();
    delete entry;
  }
}

/**
 * @brief Unifies two terms.
 * @param t1 The first term.
 * @param t2 The second term.
 * @return True if the terms unify, false otherwise.
 */
bool Engine::unify(TermPtr t1, TermPtr t2) {
  TermPtr t1_deref = t1->dereference();
  TermPtr t2_deref = t2->dereference();
  // Same pointers
  if (t1_deref == t2_deref) {
    return true;
  }
  // Same values
  if (*t1_deref == *t2_deref) {
    return true;
  }
  // Deal with variables first as this situation is more likely
  if (PVarPtr v1 = std::dynamic_pointer_cast<PVar>(t1_deref)) {
    trail(v1);
    return v1->bind(t2_deref);
  }
  if (PVarPtr v2 = std::dynamic_pointer_cast<PVar>(t2_deref)) {
    trail(v2);
    return v2->bind(t1_deref);
  }

  // Below here neither term is a variable
  if (std::dynamic_pointer_cast<PAtom>(t1_deref)) {
    // t1 is a PAtom with a different value to t2 and so they don't unify
    return false;
  }
  if (std::dynamic_pointer_cast<PAtom>(t2_deref)) {
    // t2 is a PAtom with a different value to t1 and so they don't unify
    return false;
  }
  // ditto for PInt
  if (std::dynamic_pointer_cast<PInt>(t1_deref)) {
    return false;
  }
  if (std::dynamic_pointer_cast<PInt>(t2_deref)) {
    return false;
  }
  // ditto for PFloat
  if (std::dynamic_pointer_cast<PInt>(t1_deref)) {
    return false;
  }
  if (std::dynamic_pointer_cast<PInt>(t2_deref)) {
    return false;
  }

  // If one term is a CList then the other must be for them to unify
  CListPtr l1 = std::dynamic_pointer_cast<CList>(t1_deref);
  CListPtr l2 = std::dynamic_pointer_cast<CList>(t2_deref);
  if (l1 && l2) {
    if (l1->getElements().size() != l2->getElements().size()) {
      // different sizes so they can't unify
      return false;
    }
    // unify elements pairwise
    auto it1 = l1->getElements().begin();
    auto it2 = l2->getElements().begin();
    while (it1 != l1->getElements().end()) {
      if (!unify(*it1, *it2)) {
        return false;
      }
      ++it1;
      ++it2;
    }
    return true;
  }
  if (l1 || l2) {
    // exactly one is a CList and so they can't unify
    return false;
  }
  // both are user defined types
  return t1_deref->unifyWith(this, t2_deref);
}

/**
 * @brief Pushes a predicate onto the environment stack.
 * @param p The predicate to push.
 */
void Engine::push(PredPtr p) {
  env_entry *entry = new env_entry();
  entry->pred = p;
  entry->trail_index = trail_stack.size();
  env_stack.push(entry);
}

/**
 * @brief Call a predicate.
 *
 * If the call is non-deterministic, the predicate is pushed onto the
 * environment stack before the call. Otherwise it is not pushed
 * and the call is made directly.
 *
 * @param p The predicate to call.
 * @return True if the call succeeds, false otherwise.
 */
bool Engine::call_predicate(PredPtr p) {
  if (p == nullptr) {
    return true;
  }
  if (p->is_non_det()) {
    push(p);
  }
  p->initialize_call();
  return make_choice_and_continue(p);
}

/**
 * @brief Retries the call on a predicate.
 *
 * If the predicate has no more choices, it is popped from the
 * environment stack. Otherwise, the predicate is called.
 * Note that only predicates that have been pushed onto the
 * environment stack can be retried (as they are non-deterministic).
 *
 * @param p The predicate to retry.
 */
bool Engine::retry_predicate(PredPtr p) {
  if (!p->more_choices()) {
    env_entry *entry = env_stack.top();
    env_stack.pop();
    delete entry;
    return false;
  }
  return make_choice_and_continue(p);
}

/**
 * @brief Pops the top predicate call from the environment stack.
 */
void Engine::pop_pred_call() {
  backtrack();
  env_entry *entry = env_stack.top();
  env_stack.pop();
  delete entry;
}

/**
 * @brief Cuts the environment stack to the choice point at the given index.
 * @param env_index The index to cut back to.
 */
void Engine::cut_to_choice_point(int env_index) {
  while (env_stack.size() > env_index) {
    env_entry *entry = env_stack.top();
    env_stack.pop();
    delete entry;
  }
}

/**
 * @brief Clears the environment and trail stacks.
 */
void Engine::clear_stacks() {
  while (env_stack.size() > 0) {
    backtrack();
    env_entry *entry = env_stack.top();
    env_stack.pop();
    delete entry;
  }
}

/**
 * @brief Executes a predicate.
 * @param p The predicate to execute.
 * @param unbind Whether to unbind variables after execution.
 * @return True if the execution succeeds, false otherwise.
 */
bool Engine::execute(PredPtr p, bool unbind) {
  int top_of_env_stack = env_stack.size();
  bool has_succeeded = call_predicate(p);
  while (!has_succeeded) {
    if (env_stack.size() == top_of_env_stack)
      break;
    backtrack();
    has_succeeded = retry_predicate(env_stack.top()->pred);
  }

  if (unbind)
    clear_stacks();
  return has_succeeded;
}

} // namespace pl_search