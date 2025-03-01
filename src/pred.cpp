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

/**
 * @file pred.cpp
 * @brief Implementation of the Pred class and its derived classes.
 */

#include "pl_search/pred.hpp"
#include "pl_search/choice_iterator.hpp"
#include "pl_search/engine.hpp"
#include "pl_search/term.hpp"

using namespace std;
namespace pl_search {

/**
 * @brief Initializes the predicate call.
 */
void Pred::initialize_call() {}

/**
 * @brief Follows the continuation chain to the last predicate.
 * @return A shared pointer to the last predicate in the continuation chain.
 */
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

/**
 * @brief Applies a choice for the choice predicate.
 * @return True if the choice is applied successfully, false otherwise.
 */
bool ChoicePred::apply_choice() { return choice_iterator->make_choice(); }

/**
 * @brief Tests a choice for the choice predicate.
 * @return True if the choice is valid, false otherwise.
 */
bool ChoicePred::test_choice() { return true; }

/**
 * @brief Checks if there are more choices for the choice predicate.
 * @return True if there are more choices, false otherwise.
 */
bool ChoicePred::more_choices() { return choice_iterator->has_next(); }

/**
 * @brief Creates a conjunction of predicates.
 * @param preds A vector of shared pointers to the predicates.
 * @return A shared pointer to the first predicate in the conjunction.
 */
PredPtr conjunction(std::vector<PredPtr> preds) {
  if (preds.empty())
    return nullptr;
  PredPtr first = preds.front();

  for (auto it = preds.begin(); it != prev(preds.end()); ++it) {
    PredPtr last = (*it)->last_pred();
    last->set_continuation(*(it + 1));
  }
  return first;
}

/**
 * @brief Applies a choice for the cut predicate.
 * @return True
 */
bool Cut::apply_choice() {
  engine->cut_to_choice_point(env_index);
  return true;
}

/**
 * @brief Applies a choice for the not-not-end predicate.
 * @return False
 */
bool NotNotEnd::apply_choice() {
  *succeeded = true;
  return false;
}

/**
 * @brief Initializes the call for the not-not predicate.
 */
void NotNot::initialize_call() {
  saved_continuation = continuation;
  succeeded = false;
  another_choice = true;
  PredPtr notnotend = make_shared<NotNotEnd>(engine, &succeeded);
  pred->wrap_with_once();
  continuation = pred;
  pred->last_pred()->set_continuation(notnotend);
}

/**
 * @brief Applies a choice for the not-not predicate.
 * @return True if the choice is applied successfully, false otherwise.
 */
bool NotNot::apply_choice() {
  if (another_choice) {
    return true; ///< simply succeed for the first choice
  }
  if (succeeded) {
    ///< for the second choice we test if we reached NotNotEnd
    ///< if so then the call of NotNot succeeded and so we move to the next
    ///< predicate.
    continuation = saved_continuation;
    return true;
  }
  return false;
}

/**
 * @brief Tests a choice for the not-not predicate.
 * @return True
 */
bool NotNot::test_choice() { return true; }

/**
 * @brief Checks if there are more choices for the not-not predicate.
 * @return True if there are more choices, false otherwise.
 */
bool NotNot::more_choices() {
  if (another_choice) {
    another_choice = false; ///< only two choices
    return true;
  }
  return false;
}

void Loop::initialize_call() {}

/**
 * @brief Sets the continuation for the loop predicate.
 * @param cont A shared pointer to the continuation predicate.
 */
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

/**
 * @brief Applies a choice for the loop predicate.
 * @return True if the choice is applied successfully, false otherwise.
 */
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

/**
 * @brief Tests a choice for the loop predicate.
 * @return True if the choice is valid, false otherwise.
 */
bool Loop::test_choice() { return true; }

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