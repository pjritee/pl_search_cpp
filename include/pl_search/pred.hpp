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


// Aproximating Prolog predicates in C++ using a continuation-passing style

#ifndef PL_SEARCH_PRED_HPP_
#define PL_SEARCH_PRED_HPP_

#include "term.hpp"
#include "engine.hpp"
#include "typedefs.hpp"

#include <stack>
#include <vector>
#include <memory>

// An approximation of a Prolog predicate.
// We use shared_ptr as we end up with multiple instances of loop predicates 
// that need to be cleaned up on backtracking.
// We use a continuation-passing style to approximate conjunctions in Prolog

namespace pl_search {

class Engine;
class ChoiceIterator;

class Pred : public std::enable_shared_from_this<Pred> {
public:
  Pred(Engine* eng) : engine(eng), continuation(nullptr) {}
  virtual void initialize_call() = 0;
  virtual bool apply_choice() { return false; }
  virtual bool test_choice() { return false; }
  virtual bool more_choices() { return false; }
  PredPtr get_continuation() { return continuation; }
  void set_continuation(PredPtr cont) { continuation = cont; }
  PredPtr last_pred();

  virtual ~Pred() = default; // Virtual destructor for proper cleanup

protected:
  Engine* engine;
  PredPtr continuation;
};

class ChoicePred : public Pred {
public:
  ChoiceIterator* choice_iterator;

  ChoicePred(Engine* eng, ChoiceIterator* ch) : Pred(eng), choice_iterator(ch) {}

  void initialize_call() override {}
  bool apply_choice() override;
  bool test_choice() override;
  bool more_choices() override;
};

class SemiDetPred : public Pred {
public:
  SemiDetPred(Engine* eng) : Pred(eng) {first_call = true;}

  bool more_choices() override { 
    if (first_call) {
      first_call = false;
      return true;
    }
    return false; 
  }
  
private:
  bool first_call;
};

class DetPred : public Pred {
public:
  DetPred(Engine* eng) : Pred(eng) { first_call = true; }

  bool more_choices() override { 
    if (first_call) {
      first_call = false;
      return true;
    }
    return false; 
  }
  bool apply_choice() override { return true; }
  bool test_choice() override { return true; }

  private:
    bool first_call;
};

class DisjPred : public Pred {
public:
  DisjPred(Engine* eng, std::vector<PredPtr> preds) : 
    Pred(eng), preds(preds) {}

  void initialize_call() override;
  bool apply_choice() override;
  bool test_choice() override;
  bool more_choices() override;
  void set_continuation(PredPtr cont);

private:
  std::vector<PredPtr> preds;
  std::vector<PredPtr>::iterator current_pred;
};

class OnceEnd : public Pred {
public:
  OnceEnd(Engine* eng) : Pred(eng) {
    first_call = true;
  }

  void initialize_call() override {   
  }

  bool apply_choice() override {
    return true;
    }
  
  bool test_choice() override {
    return true;
  }

  bool more_choices() override {
    if (first_call) {
      first_call = false;
      return true;
    }
    engine->pop_to_once();
    return false;
  }
  private:
    bool first_call;
};

class Once : public DetPred {
public:
  PredPtr pred;

  Once(Engine* eng, PredPtr p) : DetPred(eng), pred(p) {
    set_continuation(pred);
    PredPtr end = std::make_shared<OnceEnd>(eng);
    pred->last_pred()->set_continuation(end);
    assert(continuation == pred);
    assert(pred->get_continuation() == end);
     }

  void initialize_call() override {}


};

PredPtr conjunction(std::vector<PredPtr> preds);

} // namespace pl_search

#endif // PL_SEARCH_PRED_HPP_
