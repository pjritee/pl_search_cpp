

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
#include "pl_search/pred.hpp"
#include "pl_search/engine.hpp"
#include "pl_search/term.hpp"
#include "pl_search/choice_iterator.hpp"

using namespace std;
namespace pl_search {


bool Pred::call() {
  initialize_call();
  return try_call();
  
}

bool Pred::try_call() {
  if (!more_choices()) {
    engine->pop_call();
    return false;
  }
  if (apply_choice() && test_choice()) {
    return engine->push_and_call(get_continuation());
  }
  return false;
}
    
void Pred::initialize_call() { 
}

// follow the continuation chain to the last predicate
PredPtr Pred::last_pred() {
  PredPtr p = shared_from_this();
  while (p->get_continuation() != nullptr) {
    p = p->get_continuation();
  }
  return p;
}


bool ChoicePred::apply_choice() {
  return choice_iterator->make_choice();
}


bool ChoicePred::test_choice() {
  return true;
}

bool ChoicePred::more_choices() {
  return choice_iterator->has_next();
}


// Create a predicate that is a conjunction of a list of predicates
PredPtr conjunction(Engine* engine, std::vector<PredPtr> preds) {
  if (preds.empty()) return nullptr;
  PredPtr first = preds.front();

  for (auto it = preds.begin(); it != prev(preds.end()); ++it) {
    PredPtr last = (*it)->last_pred();
    last->set_continuation(*(it + 1));
  }
  return first;
}


void DisjPred::initialize_call() {
  current_pred = preds.begin(); 
}

bool DisjPred::apply_choice(){
  return (*current_pred)->call();
}

bool DisjPred::test_choice() {
  return true;
}

bool DisjPred::more_choices() {
  return current_pred != preds.end();
}

void DisjPred::set_continuation(PredPtr cont) {
  for (auto it = current_pred; it != preds.end(); ++it) {
    (*it)->set_continuation(cont);
  }
}

} // namespace pl_search