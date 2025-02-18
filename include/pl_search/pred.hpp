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
#include "typedefs.hpp"
// #include "engine.hpp"

#include <memory>
#include <stack>
#include <vector>

#include <iostream>
#include <sstream>

// An approximation of a Prolog predicate.
// We use shared_ptr as we end up with multiple instances of loop predicates
// that need to be cleaned up on backtracking.
// We use a continuation-passing style to approximate conjunctions in Prolog

namespace pl_search {

class Engine;
class ChoiceIterator;

class Pred : public std::enable_shared_from_this<Pred> {
public:
  Pred(Engine *eng) : engine(eng), continuation(nullptr) {}
  virtual void initialize_call() = 0;
  virtual bool apply_choice() { return false; }
  virtual bool test_choice() { return false; }
  virtual bool more_choices() { return false; }
  PredPtr get_continuation() { return continuation; }
  virtual void set_continuation(PredPtr cont) { continuation = cont; }
  PredPtr last_pred();
  bool is_non_det() { return true; }
  void wrap_with_once();

  virtual ~Pred() = default; // Virtual destructor for proper cleanup

  std::string get_name() { return typeid(this).name(); }

protected:
  PredPtr continuation;
  Engine *engine;
};

// void wrap_with_once(PredPtr);

class ChoicePred : public Pred {
public:
  ChoiceIterator *choice_iterator;

  ChoicePred(Engine *eng, ChoiceIterator *ch)
      : Pred(eng), choice_iterator(ch) {}

  void initialize_call() override {}
  bool apply_choice() override;
  bool test_choice() override;
  bool more_choices() override;
};

class SemiDetPred : public Pred {
public:
  SemiDetPred(Engine *eng) : Pred(eng) {}

  bool more_choices() override { return false; }

  bool is_non_det() { return false; }
};

class DetPred : public SemiDetPred {
public:
  DetPred(Engine *eng) : SemiDetPred(eng) {}

  bool apply_choice() override { return true; }
  bool test_choice() override { return true; }
};

PredPtr conjunction(std::vector<PredPtr> preds);

/**
 * The DisjPred class is an implementation of disjunctionin Prolog.
 * In Prolog we might write p1 ; p2 ; p3. Here we would provide the
 * constructor with the vector {p1, p2, p3}. Initially the continuation
 * for the object and each of p1, p2, p3 is nullptr but if it becomes
 * part of a conjunction then the set_continuation method sets
 * the (ultimate) continuation of each of p1,p2,p3 to the next
 * predicate in the conjunction. This means than when each disjunct
 * is chosen the execution moves on to the next predicate in the
 * conjunction if the call succeeds.
 */
class DisjPred : public Pred {
public:
  DisjPred(Engine *eng, std::vector<PredPtr> preds) : Pred(eng), preds(preds) {}

  void initialize_call() override;
  bool apply_choice() override;
  bool test_choice() override;
  bool more_choices() override;
  bool is_non_det() { return true; }
  void set_continuation(PredPtr cont);

private:
  std::vector<PredPtr> preds;
  std::vector<PredPtr>::iterator current_pred;
};

class Cut : public DetPred {
public:
  Cut(Engine *eng, int index) : DetPred(eng), env_index(index) {}

  void initialize_call() override {}

  bool apply_choice() override;

  bool test_choice() override { return true; }

private:
  int env_index;
};

class NotNotEnd : public SemiDetPred {
public:
  NotNotEnd(Engine *eng, bool *succ) : SemiDetPred(eng), succeeded(succ) {}

  void initialize_call() override {};
  bool apply_choice() override;
  bool test_choice() override { return true; };

private:
  bool *succeeded;
};

class NotNot : public Pred {
public:
  NotNot(Engine *eng, PredPtr p) : Pred(eng), pred(p) {}

  void initialize_call() override;
  bool apply_choice() override;
  bool test_choice() override;
  bool more_choices() override;

private:
  PredPtr pred;
  bool succeeded;
  bool another_choice;
  PredPtr saved_continuation;
};

/**
 * LoopBodyFactory is an abstract base class used for  generating
 * instances of a predicate class used in the body of a loop.
 */
class LoopBodyFactory {
public:
  Engine *engine;

  LoopBodyFactory(Engine *eng) : engine(eng) {}

  virtual ~LoopBodyFactory(){};

  virtual bool loop_continues() = 0;

  virtual PredPtr make_body_pred() = 0;
};

class Loop : public DetPred {
public:
  LoopBodyFactory *body_factory;

  Loop(Engine *eng, LoopBodyFactory *bf) : DetPred(eng), body_factory(bf) {}

  void initialize_call() override;
  bool apply_choice() override;
  bool test_choice() override;
  void set_continuation(PredPtr cont);

private:
  PredPtr saved_continuation;
};

// for testing
std::string repr(PredPtr pred);

} // namespace pl_search

#endif // PL_SEARCH_PRED_HPP_
