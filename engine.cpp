#include "engine.hpp"

#include <fstream>
#include <iostream>

using namespace std;

void Engine::trail(Term* v) {

  // shared_ptr<trail_entry> entry(new trail_entry);
  // entry->var = v;
  // entry->value = v->value;
  trail_stack.push(v);
}

void Engine::backtrack() {
  int old_top = env_stack.top()->trail_index;
  while (trail_stack.size() > old_top) {
    Term* ptr =  trail_stack.top();
    ptr->reset();
    trail_stack.pop();
  }
}

bool Engine::unify(Term* t1, Term* t2) {
  Term* t1_deref = t1->dereference();
  Term* t2_deref = t2->dereference();
  if (t1_deref->kind == VAR) {
    t1_deref->bind(t2_deref);
    trail(t1_deref);
    return true;
  }
  if (t2_deref->kind == VAR) {
    t2_deref->bind(t1_deref);
    trail(t2_deref);
    return true;
  }  
  return (*t1_deref == *t2_deref);
}
bool Engine::unify(Term* t1,TermList t2) {
  Term* t1_deref = t1->dereference();
  if (t1_deref->kind == VAR) {
    t1_deref->bind(t2);
    trail(t1_deref);
    return true;
  }
  return false;
}

bool Engine::push_and_call(PredPtr p) {
  if (p == nullptr) {
    return true;
  }
  shared_ptr<env_entry> entry(new env_entry);
  entry->pred = p;
  entry->trail_index = trail_stack.size();
  env_stack.push(entry);
  return p->call();
}

void Engine::pop_call() {
  backtrack();
  env_stack.pop();
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

// int main()
// {
//   Var v1 = Var();
//   Var v2 = Var();
//   Int i1(42);
//   Engine eng;
//   eng.unify(v1, v2);
//   eng.unify(v2, i1);
//   cout << v1.dereference() << " " << v2.dereference() << endl;

//   eng.backtrack();
//   cout << v1.dereference() << " " << v2.dereference() << endl;

  


//   return 1;
// }
