

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

void Pred::wrap_with_once() {
  int env_index = engine->env_stack.size();
  PredPtr cut_pred = make_shared<Cut>(engine, env_index);
  last_pred()->set_continuation(cut_pred);
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
PredPtr conjunction(std::vector<PredPtr> preds) {
  if (preds.empty()) return nullptr;
  PredPtr first = preds.front();

  for (auto it = preds.begin(); it != prev(preds.end()); ++it) {
    PredPtr last = (*it)->last_pred();
    last->set_continuation(*(it + 1));
  }
  return first;
}

bool Cut::apply_choice() {
  engine->cut_to_choice_point(env_index);
  return true;
}

void DisjPred::initialize_call() {
  current_pred = preds.begin(); 
}

bool DisjPred::apply_choice(){
  PredPtr cont = *current_pred;
  // the chosen disjunct should have the same continuation as the disjunction
  continuation = cont;
  ++current_pred;
  return true;
}

bool DisjPred::test_choice() {
  return true;
}

bool DisjPred::more_choices() {
  return current_pred != preds.end();
}

void DisjPred::set_continuation(PredPtr cont) {
  for (auto it = preds.begin(); it != preds.end(); ++it) {
    (*it)->last_pred()->set_continuation(cont);
    
  }
  
};

bool NotNotEnd::apply_choice() {
  *succeeded = true;
  return false;
}

void NotNot::initialize_call() {
  saved_continuation = continuation;
  succeeded = false;
  another_choice = true;
  PredPtr notnotend = make_shared<NotNotEnd>(engine, &succeeded);
  pred->wrap_with_once();
  continuation = pred;
  pred->last_pred()->set_continuation(notnotend);
}

bool NotNot::apply_choice() {
 if (another_choice) {
  return true;
 }
 if (succeeded) {
  continuation = saved_continuation;
  return true;
 }
 return false;
}

bool NotNot::test_choice() { return true; }


bool NotNot::more_choices() {
  if (another_choice) {
    another_choice = false;
    return true;
  }
  return false;
}


void Loop::initialize_call() {

}

void Loop::set_continuation(PredPtr cont) {
  /**
  * When the loop body predicate is created in apply_choice,
  * continuation is set to the newely created predicate.
  * When the loop exits continuation is reset to the original continuation 
  * (the predicate to call after the loop body). Therefore the set continuation
  * needs to be saved.
   */
  continuation = cont;
  saved_continuation = cont;
}

bool Loop::apply_choice() {
  if (body_factory->loop_continues()) {
    PredPtr pred = body_factory->make_body_pred();
    pred->last_pred()->set_continuation(shared_from_this());
    continuation = pred;
  } else {
    continuation = saved_continuation;
  }
  return true;
}

bool Loop::test_choice() {
  return true;

}


// for debugging
std::string repr(PredPtr pred) {
  std::stringstream s;
    while (pred->get_continuation() != nullptr) {
      s << pred << " - ";
      pred = pred->get_continuation();
    }
  	s << pred << " - nullptr";
		return s.str();
	}

} // namespace pl_search