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
//#include "pl_search/choice_iterator.hpp"
#include "pl_search/typedefs.hpp"
#include "pl_search/pred.hpp"
#include "pl_search/engine.hpp"

#include <fstream>
#include <iostream>

using namespace std;

namespace pl_search {


void Engine::trail(PVar* v) {

  shared_ptr<trail_entry> entry(new trail_entry);
  entry->var = v;
  entry->value = v->value;
  trail_stack.push(entry);
}

void Engine::backtrack() {
  int old_top = env_stack.top()->trail_index;
 
  while (trail_stack.size() > old_top) {
    shared_ptr<trail_entry> entry =  trail_stack.top();
    entry->var->reset(entry->value);
    trail_stack.pop();
  }
}

bool Engine::unify(Term* t1, Term* t2) {
  Term* t1_deref = t1->dereference();
  Term* t2_deref = t2->dereference();
  // Same pointers
  if (t1_deref == t2_deref) {
    return true;
  }
  // Same values
  if (*t1_deref == *t2_deref) {
    return true;
  }
  if (PVar* v1 = dynamic_cast<PVar*>(t1_deref)) {
    trail(v1);
    v1->bind(t2_deref);
    return true;
  }
  if (PVar* v2 = dynamic_cast<PVar*>(t2_deref)) {
    trail(v2);
    v2->bind(t1_deref);
    return true;
  }
  CList* l1 = dynamic_cast<CList*>(t1_deref);
  CList* l2 = dynamic_cast<CList*>(t2_deref);
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

  return t1_deref->unifyWith(t2_deref);
}

void Engine::push(PredPtr p) {
  shared_ptr<env_entry> entry(new env_entry);
  entry->pred = p;
  entry->trail_index = trail_stack.size();
  env_stack.push(entry);
} 

bool Engine::push_and_call(PredPtr p) {
  if (p == nullptr) {
    return true;
  }
  push(p);
  return p->call();
}

void Engine::pop_call() {
  backtrack();
  env_stack.pop();
}

void Engine::pop_to_once() {
  while (typeid(*(env_stack.top()->pred)) != typeid(Once)) {
    env_stack.pop();
  }
}

void Engine::clear_stacks() {
   while (env_stack.size() > 0) {
     //PredPtr pred_call = env_stack.top()->pred;
     //pred_call->cleanup();
     backtrack();
     env_stack.pop();
   }
   // while (trail_stack.size() > 1) {
   //   Term* ptr =  trail_stack.top();
   //   cerr << "clear " << *ptr << endl;
   //   ptr->reset();
   //   trail_stack.pop();
   // }
}
  
bool Engine::execute(PredPtr p, bool unbind) {
  int top_of_env_stack = env_stack.size();
  bool has_succeeded = push_and_call(p);
  while (!has_succeeded) {
    if (env_stack.size() == top_of_env_stack) break;
    backtrack();
    PredPtr pred_call = env_stack.top()->pred;
    has_succeeded = pred_call->try_call();
  }
  if (unbind) clear_stacks();
  return has_succeeded;
}

} // namespace pl_search