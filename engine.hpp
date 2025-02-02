#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include "term.hpp"
#include "pred.hpp"

#include <stack>
#include <memory>

//using namespace std;
//class Pred;

// struct trail_entry {
//   Var_ptr var;
//   Term_ptr value;
// };


struct env_entry {
  PredPtr pred;
  int trail_index;
};

class Engine {
public:

  stack<Term*> trail_stack;

  stack<shared_ptr<env_entry>> env_stack;
  
  void trail(Term* v);

  void backtrack();

  bool unify(Term* v, Term* t);
  bool unify(Term* v, TermList t);

  bool execute(PredPtr p, bool unbind);

  bool push_and_call(PredPtr p);

  void pop_call();

  void clear_stacks();
};


#endif
