


#include "engine.hpp"
#include "term.hpp"
#include "pred.hpp"


using namespace std;

Pred::Pred(Engine* eng, PredPtr cont) {
    engine = eng;
    continuation = cont;
}

bool Pred::call() {
  initialize_call();
  return try_call();
  
}

bool Pred::try_call() {
  if (more_choices()) {
    if (apply_choice() && test_choice()) {
      return engine->push_and_call(get_continuation());
    }
    return false;
  }
  engine->pop_call();
  return false;
}
    
void Pred::initialize_call() {
 
}



bool Pred::apply_choice() {
  return false;
}

bool Pred::test_choice() {
  return false;
}

bool Pred::more_choices() {
  return false;
}

PredPtr Pred::get_continuation() {
  return nullptr;
}


ChoicePred::ChoicePred(Engine* eng, PredPtr cont, Term* v,
            stack<Term*> &ch): Pred(eng,cont){
   var = v;
   choices = ch;
 }

void ChoicePred::initialize_call() {
}

bool ChoicePred::apply_choice() {
  Term* t = choices.top();
  choices.pop();
  return engine->unify(var, t);
}



bool ChoicePred::test_choice() {
  return true;
}

bool ChoicePred::more_choices() {
  return !choices.empty();
}

PredPtr ChoicePred::get_continuation() {
  return continuation;
}
