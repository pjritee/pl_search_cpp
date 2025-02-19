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
void Engine::trail(PVar *v) {
  shared_ptr<trail_entry> entry(new trail_entry);
  entry->var = v;
  entry->value = v->value;
  trail_stack.push(entry);
}

/**
 * @brief Performs backtracking.
 */
void Engine::backtrack() {
  int old_top;
  ///< If the current execution does not involve non-deterministic
  ///< predicates then there will be no choices points (env_stack will be
  ///< empty). In that case we should remove all variable bindings on the trail.
  if (env_stack.empty()) {
    old_top = 0;
  } else {
    old_top = env_stack.top()->trail_index;
  }

  while (trail_stack.size() > old_top) {
    shared_ptr<trail_entry> entry = trail_stack.top();
    entry->var->reset(entry->value);
    trail_stack.pop();
  }
}

/**
 * @brief Unifies two terms.
 * @param t1 The first term.
 * @param t2 The second term.
 * @return True if the terms unify, false otherwise.
 */
bool Engine::unify(Term *t1, Term *t2) {
  Term *t1_deref = t1->dereference();
  Term *t2_deref = t2->dereference();
  ///< Same pointers
  if (t1_deref == t2_deref) {
    return true;
  }
  ///< Same values
  if (*t1_deref == *t2_deref) {
    return true;
  }
  if (PVar *v1 = dynamic_cast<PVar *>(t1_deref)) {
    trail(v1);
    v1->bind(t2_deref);
    return true;
  }
  if (PVar *v2 = dynamic_cast<PVar *>(t2_deref)) {
    trail(v2);
    v2->bind(t1_deref);
    return true;
  }
  CList *l1 = dynamic_cast<CList *>(t1_deref);
  CList *l2 = dynamic_cast<CList *>(t2_deref);
  if ((l1 != nullptr) && (l2 != nullptr)) {
    if (l1->getElements().size() != l2->getElements().size()) {
      return false;
    }
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

  return t1_deref->unifyWith(this, t2_deref);
}

/**
 * @brief Pushes a predicate onto the environment stack.
 * @param p The predicate to push.
 */
void Engine::push(PredPtr p) {
  shared_ptr<env_entry> entry(new env_entry);
  entry->pred = p;
  entry->trail_index = trail_stack.size();
  env_stack.push(entry);
}

/**
 * @brief Calls a predicate.
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
 *
 * @param p The predicate to retry.
 * @return True if the retry succeeds, false otherwise.
 */
bool Engine::retry_predicate(PredPtr p) {
  if (!p->more_choices()) {
    env_stack.pop();
    return false;
  }
  return make_choice_and_continue(p);
}

/**
 * @brief Makes a choice and continues execution.
 * @param p The predicate to continue with.
 * @return True if the continuation succeeds, false otherwise.
 */
bool Engine::make_choice_and_continue(PredPtr p) {
  if (p->apply_choice() && p->test_choice()) {
    return call_predicate(p->get_continuation());
  }
  return false;
}

/**
 * @brief Cuts the environment stack to a specific choice point.
 * @param env_index The index of the choice point.
 */
void Engine::cut_to_choice_point(int env_index) {
  while (env_stack.size() > env_index) {
    env_stack.pop();
  }
}

/**
 * @brief Clears the environment and trail stacks.
 */
void Engine::clear_stacks() {
  while (env_stack.size() > 0) {
    backtrack();
    env_stack.pop();
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
    PredPtr pred_call = env_stack.top()->pred;
    has_succeeded = retry_predicate(pred_call);
  }

  if (unbind)
    clear_stacks();
  return has_succeeded;
}

} // namespace pl_search