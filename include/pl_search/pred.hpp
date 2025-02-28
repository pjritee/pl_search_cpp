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

#include <memory>
#include <stack>
#include <vector>

/**
 * @file pred.hpp
 * @brief Definition of the Pred class and its derived classes.
 */

namespace pl_search {

class ChoiceIterator;

/**
 * @brief Abstract base class for Prolog-like predicates.
 *
 * The Pred class provides an interface for Prolog-like predicates. It includes
 * methods for initializing calls, applying choices, testing choices, and
 * checking for more choices. It also includes methods for managing
 * continuations.
 */
class Pred : public std::enable_shared_from_this<Pred> {
public:
  /**
   * @brief Default constructor.
   */
  Pred(Engine *eng) : engine(eng), continuation(nullptr) {}

  /**
   * @brief Initializes the predicate call.
   */
  virtual void initialize_call() = 0;

  /**
   * @brief Applies a choice.
   * @return True if the choice is applied successfully, false otherwise.
   */
  virtual bool apply_choice() { return false; }

  /**
   * @brief Tests a choice.
   * @return True if the choice is valid, false otherwise.
   */
  virtual bool test_choice() { return false; }

  /**
   * @brief Checks if there are more choices.
   * @return True if there are more choices, false otherwise.
   */
  virtual bool more_choices() { return false; }

  /**
   * @brief Gets the continuation of the predicate.
   * @return A shared pointer to the continuation predicate.
   */
  PredPtr get_continuation() { return continuation; }

  /**
   * @brief Sets the continuation of the predicate.
   * @param cont A shared pointer to the continuation predicate.
   */
  virtual void set_continuation(PredPtr cont) { continuation = cont; }

  /**
   * @brief Follows the continuation chain to the last predicate.
   * @return A shared pointer to the last predicate in the continuation chain.
   */
  PredPtr last_pred();

  /**
   * @brief Determines if the predicate is non-deterministic.
   * @return True if the predicate is non-deterministic, false otherwise.
   */
  bool is_non_det() { return true; }

  /**
   * @brief Wraps the predicate with a once.
   */
  void wrap_with_once();

  /**
   * @brief Virtual destructor for proper cleanup.
   */
  virtual ~Pred() = default;

  std::string get_name() { return typeid(this).name(); }

protected:
  PredPtr continuation; ///< The continuation of the predicate.
  Engine *engine;
};

typedef std::shared_ptr<ChoiceIterator> ChoiceIteratorPtr;

/**
 * @brief Represents a choice predicate.
 */
class ChoicePred : public Pred {
public:
  ChoiceIteratorPtr choice_iterator; ///< Pointer to the choice iterator.

  /**
   * @brief Constructs a ChoicePred with the given choice iterator.
   * @param ch Pointer to the choice iterator.
   */
  ChoicePred(Engine *eng, ChoiceIteratorPtr ch)
      : Pred(eng), choice_iterator(ch) {}

  void initialize_call() override {}
  bool apply_choice() override;
  bool test_choice() override;
  bool more_choices() override;
};

/**
 * @brief Represents a semi-deterministic predicate.
 */
class SemiDetPred : public Pred {
public:
  /**
   * @brief Default constructor.
   */
  SemiDetPred(Engine *eng) : Pred(eng) {}

  bool more_choices() override { return false; }

  bool is_non_det() { return false; }
};

/**
 * @brief Represents a deterministic predicate.
 */
class DetPred : public SemiDetPred {
public:
  /**
   * @brief Default constructor.
   */
  DetPred(Engine *eng) : SemiDetPred(eng) {}

  bool apply_choice() override { return true; }
  bool test_choice() override { return true; }
};

/**
 * @brief Represents a conjunction of predicates by chaining them together via
 * continuations.
 * @param preds A vector of shared pointers to the predicates.
 * @return A shared pointer to the first predicate in the conjunction.
 */
PredPtr conjunction(std::vector<PredPtr> preds);

/**
 * @brief Represents a disjunction of predicates.
 */
class DisjPred : public Pred {
public:
  /**
   * @brief Constructs a DisjPred with the given predicates.
   * @param preds A vector of shared pointers to the predicates.
   *
   * preds become the list of choices - when a choice is made that
   * choice becomes the next predicate to be called.
   */
  DisjPred(Engine *eng, std::vector<PredPtr> preds)
      : Pred(eng), preds(preds), index(0) {}

  void initialize_call() override {
    ///< Set the continuations for each of preds to this continuation
    for (auto it = preds.begin(); it != preds.end(); it++) {
      (*it)->set_continuation(continuation);
    }
  };

  bool apply_choice() override {
    ///< Get the next choice
    PredPtr choice = preds[index++];
    /**
     * By setting continuation to choice, it will be the next predicate
     * to be called
     */
    continuation = choice;
    return true;
  }

  bool test_choice() override { return true; }

  bool more_choices() override { return index < preds.size(); }

private:
  std::vector<PredPtr> preds; ///< The predicates in the disjunction.
  int index;                  // index of the current predicate choice
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