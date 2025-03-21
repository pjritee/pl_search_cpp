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
  virtual void initialize_call() {};

  /**
   * @brief Applies a choice.
   * @return True if the choice is applied successfully, false otherwise.
   */
  virtual bool apply_choice() { return false; }

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

  /**
   * @brief For debugging
   */
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
   * @param eng Pointer to the engine.
   * @param ch Pointer to the choice iterator.
   */
  ChoicePred(Engine *eng, ChoiceIteratorPtr ch)
      : Pred(eng), choice_iterator(ch) {}

  void initialize_call() override {}
  bool apply_choice() override;
  bool more_choices() override;
};

/**
 * @brief Represents a semi-deterministic predicate.
 */
class SemiDetPred : public Pred {
public:
  /**
   * @brief Represents a semi-deterministic predicate.
   * @param eng Pointer to the engine.
   */
  SemiDetPred(Engine *eng) : Pred(eng) {}

  /**
   * @brief  Applies a choice.
   * @return False
   */
  bool more_choices() override { return false; }

  bool is_non_det() { return false; }
};

/**
 * @brief Represents a deterministic predicate.
 */
class DetPred : public SemiDetPred {
public:
  /**
   * @brief Represents a deterministic predicate.
   * @param eng Pointer to the engine.
   */
  DetPred(Engine *eng) : SemiDetPred(eng) {}

  /**
   * @brief Noop - all the work is done in initialize_call.
   * @return True
   */
  bool apply_choice() override { return true; }
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
   * @param eng Pointer to the engine.
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

  bool more_choices() override { return index < preds.size(); }

protected:
  std::vector<PredPtr> preds; ///< The predicates in the disjunction.
  int index;                  ///< index of the current predicate choice
};

/**
 * @brief Represents a Prolog like cut. When called it pops env_stack
 * thus removing choicepoints.
 */
class Cut : public DetPred {
public:
  /**
   * @brief Constructs a Cut predicate.
   * @param eng Pointer to the engine.
   * @param index An index into env_stack. When called, env_stack is popped
   * to index.
   */
  Cut(Engine *eng, int index) : DetPred(eng), env_index(index) {}

  void initialize_call() override {}

  bool apply_choice() override;

  void set_cut_point(int cut_point) { env_index = cut_point; }

protected:
  int env_index;
};

/**
 * @brief Intended for internal use by NotNot which injects this call
 * directly after the predicate supplied to the NotNot constructor.
 */
class NotNotEnd : public SemiDetPred {
public:
  /**
   * @brief NotNot::initialize_call uses this to "terminate" the call
   * supplied to NotNot.
   * @param eng Pointer to the engine.
   * @param succ is used to flag if this object is called - i.e. the
   * call to NotNot succeeded.
   */
  NotNotEnd(Engine *eng, bool *succ) : SemiDetPred(eng), succeeded(succ) {}

  void initialize_call() override {};
  bool apply_choice() override;

protected:
  bool *succeeded;
};

/**
 * @brief Represents an equivalent of the Prolog call \\+\\+Call. The aim
 * is to determine if Call succeeds without binding any variables in the call.
 */
class NotNot : public Pred {
public:
  /**
   * @brief Create the equivalent of Prolog's \\+\\+p call.
   * @param eng Pointer to the engine.
   * @param p is the predicate called within NotNot.
   */
  NotNot(Engine *eng, PredPtr p) : Pred(eng), pred(p) {}

  void initialize_call() override;
  bool apply_choice() override;
  bool more_choices() override;

protected:
  PredPtr pred;
  bool succeeded;
  bool another_choice;
  PredPtr saved_continuation;
};

/**
* @brief Represents the equivalent of Prolog if-then-else i.e.
* (G1 -> G2; G3)  - similar to (G1, !, G2; G3)
) */
class IfThenElse : public Pred {
public:
  /**
   * @brief The equlvalent of the Prolog call (If -> Then ; Else)
   * @param eng Pointer to the engine
   * @param if_pred The guard predicate
   * @param then_pred The then predicate
   * @param else_pred The else predicate
   */
  IfThenElse(Engine *eng, PredPtr if_pred, PredPtr then_pred,
             PredPtr else_pred);

  void initialize_call() override;
  bool apply_choice() override;
  bool more_choices() override;
  void set_continuation(PredPtr cont);

protected:
  PredPtr if_pred;
  PredPtr then_pred;
  PredPtr else_pred;
  PredPtr if_then_pred;
  std::shared_ptr<Cut> cut_pred;
  int choice_number;
};
/**
 * @brief LoopBodyFactory is an abstract base class used for  generating
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

/**
 * @brief Constructs a predicate that loops over (instances of) a predicate
 * while some condition (loop_continues) is satisfied.
 */
class Loop : public DetPred {
public:
  LoopBodyFactory *body_factory;

  /**
   * @brief Constructs a predicate that loops over (instances of) a predicate
   * while some condition (loop_continues) is satisfied.
   * @param eng Pointer to the engine.
   * @param bf a subclass of LoopBodyFactory that is used to generate
   * body predicates to call and to test if the loop should continue.
   */
  Loop(Engine *eng, LoopBodyFactory *bf) : DetPred(eng), body_factory(bf) {}

  void initialize_call() override;
  bool apply_choice() override;
  void set_continuation(PredPtr cont);

protected:
  PredPtr saved_continuation;
};

// for testing
std::string repr(PredPtr pred);

} // namespace pl_search

#endif // PL_SEARCH_PRED_HPP_