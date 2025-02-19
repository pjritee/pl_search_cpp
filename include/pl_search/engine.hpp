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

#ifndef PL_SEARCH_ENGINE_HPP_
#define PL_SEARCH_ENGINE_HPP_

#include "clist.hpp"
#include "pred.hpp"
#include "pvar.hpp"
#include "term.hpp"
#include "typedefs.hpp"

#include <memory>
#include <stack>

/**
 * @file engine.hpp
 * @brief Definition of the Engine class and related structures.
 */

class EngineTest;

namespace pl_search {

/**
 * @brief Represents a trail entry for backtracking.
 */
struct trail_entry {
  PVar *var;   ///< The variable being trailed.
  Term *value; ///< The value of the variable.
};

/**
 * @brief Represents an environment entry for predicates.
 */
struct env_entry {
  PredPtr pred;    ///< The predicate being executed.
  int trail_index; ///< The index in the trail stack.
};

/**
 * @brief The Engine class manages the execution of predicates and backtracking.
 *
 * Execution uses a continuation-passing style for predicates that
 * represent a conjunction of goals. The engine implements
 * backtracking and choice points.
 */
class Engine {
public:
  Engine(){};

  /**
   * @brief Unifies two terms.
   * @param t1 The first term.
   * @param t2 The second term.
   * @return True if the terms unify, false otherwise.
   */
  bool unify(Term *t1, Term *t2);

  /**
   * @brief Executes a predicate.
   * @param p The predicate to execute.
   * @param unbind Whether to unbind variables after execution.
   * @return True if the execution succeeds, false otherwise.
   */
  bool execute(PredPtr p, bool unbind);

  friend class Cut;
  friend void Pred::wrap_with_once();

  friend class DisjPred;
  friend class ::EngineTest;

private:
  std::stack<std::shared_ptr<trail_entry>>
      trail_stack; ///< Stack for trail entries.
  std::stack<std::shared_ptr<env_entry>>
      env_stack; ///< Stack for environment entries.

  /**
   * @brief Trails a variable.
   * @param v The variable to trail.
   */
  void trail(PVar *v);

  /**
   * @brief Performs backtracking.
   */
  void backtrack();

  /**
   * @brief Call a predicate.
   *
   * If the call is non-deterministic, the predicate is pushed onto the
   * environment stack before the call. Otherwise the is not pushed
   * and the call is made directly.
   *
   * @param p The predicate to call.
   * @return True if the call succeeds, false otherwise.
   */
  bool call_predicate(PredPtr p);

  /**
   * @brief Retries the call on a predicate.
   *
   * If the predicate has no more choices, it is popped from the
   * environment stack. Otherwise, the predicate is called.
   * Note that only predicates that have been pushed onto the
   * environment stack can be retried (as they are non-deterministic).
   *
   * @param p The predicate to retry.
   */
  bool retry_predicate(PredPtr p);

  /**
   * @brief Makes the current choice, checks it, and continues
   * execution (using the predicates continuation) if the choice is valid.
   * @param p The predicate to make a choice on.
   * @return True if the choice is valid and the predicates continuation
   * succeeds, false otherwise.
   */
  bool make_choice_and_continue(PredPtr p);

  /**
   * @brief Pushes a predicate onto the environment stack.
   * @param p The predicate to push.
   */
  void push(PredPtr p);

  /**
   * @brief Pops the top predicate call from the environment stack.
   */
  void pop_pred_call();

  /**
   * @brief Pop env_stack back to the given index.
   * @param index The index to pop back to.
   */
  void cut_to_choice_point(int env_index);

  /**
   * @brief Clears the environment and trail stacks.
   */
  void clear_stacks();
};

} // namespace pl_search

#endif // PL_SEARCH_ENGINE_HPP_
