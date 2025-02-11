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

// An approximation of a Prolog predicate.
// We use shared_ptr as we end up with multiple instances of loop predicates 
// that need to be cleaned up on backtracking.
// We use a continuation-passing style to approximate conjunctions in Prolog

namespace pl_search {


class Engine;
class ChoiceIterator;

class Pred : std::enable_shared_from_this<Pred> {
public:
  Engine* engine;
  
  bool call();
  bool try_call();
  virtual void initialize_call() = 0;
  bool apply_choice() { return false; }
  bool test_choice() { return false; }
  bool more_choices() { return false; }
  PredPtr get_continuation() { return continuation; }
  void set_continuation(PredPtr cont) { continuation = cont; }
  PredPtr last_pred();

  Pred(Engine* eng) : engine(eng) { continuation = nullptr; }
 
 
  virtual ~Pred() = default; // Virtual destructor for proper cleanup

 
private: 

  PredPtr continuation;

};


class ChoicePred : public Pred {
 public:
  ChoiceIterator* choice_iterator;
 
  void initialize_call() override {};
  bool apply_choice();
  bool test_choice();
  bool more_choices();
  

  
 ChoicePred(Engine* eng, ChoiceIterator* ch) : 
  Pred(eng), choice_iterator(ch) {}
 
};

// A predicate that is semi-deterministic I.e. it has at most one solution
class SemiDetPred : public Pred {
 public:
  SemiDetPred(Engine* eng) : Pred(eng) {}

  bool try_call() {
    engine->pop_call();
    return test_choice() && engine->push_and_call(get_continuation());
  }

};

// A predicate that is deterministic I.e. it has exactly one solution
class DetPred : public Pred {
 public:
  DetPred(Engine* eng) : Pred(eng) {}

  // All the work should be done in initialize_call. The predicate succeeds
  bool try_call() {
    engine->pop_call();
    return engine->push_and_call(get_continuation());
  }
};

// Create a predicate that is a conjunction of a list of predicates
PredPtr conjunction(Engine* engine, std::vector<PredPtr> preds); 


class DisjPred : public Pred {
 public:
  DisjPred(Engine* eng, std::vector<pl_search::PredPtr> preds) :
    Pred(eng), preds(preds) {}
  void initialize_call() override;
  bool apply_choice();
  bool test_choice();
  bool more_choices();
  void set_continuation(PredPtr cont);
 private:
  std::vector<pl_search::PredPtr> preds;
  std::vector<pl_search::PredPtr>::iterator current_pred;
};


class OnceEnd : public DetPred {
 public:
  OnceEnd(Engine* eng) : DetPred(eng){}
  bool try_call(){
    engine->pop_to_once();
    return engine->push_and_call(get_continuation());
  }

  void initialize_call() override {}
};

class Once : public DetPred {
 public:
 PredPtr pred;
  Once(Engine* eng, PredPtr p) : DetPred(eng), pred(p) {}

  void initialize_call() override {
    // set preds last continuation to a OnceEnd predicate
    pred->last_pred()->set_continuation(std::make_shared<OnceEnd>(engine));
  }
  bool try_call(){
    return engine->push_and_call(pred);
  }
};



} // namespace pl_search

#endif // PL_SEARCH_PRED_HPP_
