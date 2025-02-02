#ifndef PRED_HPP_
#define PRED_HPP_
#include "term.hpp"
#include "engine.hpp"
#include <stack>
#include <memory>

class Engine;

class Pred;

typedef shared_ptr<Pred> PredPtr;

class Pred {
public:
  Engine* engine;
  PredPtr continuation;
  bool call();
  bool try_call();
  virtual void initialize_call();
  virtual bool apply_choice();
  virtual bool test_choice();
  virtual bool more_choices();
  virtual PredPtr get_continuation();

  Pred(Engine* eng, PredPtr cont);
};


class ChoicePred : public Pred {
 public:
  Term* var;
  stack<Term*> choices;
 
 void initialize_call();
  bool apply_choice();
  bool test_choice();
  bool more_choices();
  PredPtr get_continuation();
 
 ChoicePred(Engine* eng, PredPtr cont, Term* v,
            stack<Term*> &ch);
 
};



#endif
