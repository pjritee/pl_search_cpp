

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
