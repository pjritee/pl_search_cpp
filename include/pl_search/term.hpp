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

#ifndef PL_SEARCH_TERM_HPP
#define PL_SEARCH_TERM_HPP

#include <iostream>
#include <string>

/**
 * @file term.hpp
 * @brief Definition of the Term class.
 */

namespace pl_search {

class Engine;

/**
 * @brief Abstract base class for terms that approximate Prolog terms.
 *
 * The Term class provides an interface for Prolog-like terms. It includes
 * methods for dereferencing, binding, resetting, and representing terms as
 * strings. It also includes methods for comparing terms and unifying
 * user-defined classes.
 */
class Term {
public:
  /**
   * @brief Dereferences the term.
   * @return A pointer to the dereferenced term.
   */
  virtual Term *dereference() = 0;

  /**
   * @brief Binds the term to another term.
   * @param t The term to bind to.
   * @return True if the binding is successful, false otherwise.
   */
  virtual bool bind(Term *t) = 0;

  /**
   * @brief Resets the term.
   * @param t The term to reset to.
   */
  virtual void reset(Term *t) = 0;

  /**
   * @brief Returns a string representation of the term.
   * @return A string representation of the term.
   */
  virtual std::string repr() const { return "TERM"; }

  /**
   * @brief Checks if the term is equal to another term.
   * @param t The term to compare to.
   * @return True if the terms are equal, false otherwise.
   */
  virtual bool isEqualTo(Term &t) = 0;

  /**
   * @brief Checks if the term is less than another term.
   * @param t The term to compare to.
   * @return True if the term is less than the other term, false otherwise.
   */
  virtual bool isLessThan(Term &t) = 0;

  /**
   * @brief A hook for unification of user-defined classes.
   * Only override this method if you want to unify user-defined classes.
   * @param engine Pointer to the Engine.
   * @param t The term to unify with.
   * @return True if the unification is successful, false otherwise.
   */
  virtual bool unifyWith(Engine *engine, Term *t) { return false; }

  /**
   * @brief Checks if the term is a variable.
   * @return True if the term is a variable, false otherwise.
   */
  virtual bool is_var() { return false; }

  /**
   * @brief Default constructor.
   */
  Term() {}

  /**
   * @brief Virtual destructor for proper cleanup.
   */
  virtual ~Term() {} // Virtual destructor

  /**
   * @brief Overloaded output stream operator for Term.
   * @param os The output stream.
   * @param t The term to output.
   * @return The output stream.
   */
  friend std::ostream &operator<<(std::ostream &os, const Term &t) {
    os << t.repr();
    return os;
  }

  /**
   * @brief Overloaded equality operator for Term.
   * @param t1 The first term.
   * @param t2 The second term.
   * @return True if the terms are equal, false otherwise.
   */
  friend bool operator==(Term &t1, Term &t2) {
    Term *deref1 = t1.dereference();
    Term *deref2 = t2.dereference();
    return deref1->isEqualTo(*deref2);
  }

  /**
   * @brief Overloaded less-than operator for Term.
   * Approximates the @< operator in Prolog.
   * The ordering is as follows:
   * pvar < pint, pfloat < patom < clist < user-defined classes
   * "older" vars are less than "newer" vars
   * patoms are ordered by name
   * pints and pfloats are ordered by value
   * clists are ordered by the first element, then the second, etc.

   * @param t1 The first term.
   * @param t2 The second term.
   * @return True if the first term is less than the second term, false
   * otherwise.
   */
  friend bool operator<(Term &t1, Term &t2) { return t1.isLessThan(t2); }

  /**
   * @brief Overloaded less-than-or-equal-to operator for Term.
   * @param t1 The first term.
   * @param t2 The second term.
   * @return True if the first term is less than or equal to the second term,
   * false otherwise.
   */
  friend bool operator<=(Term &t1, Term &t2) {
    return t1.isLessThan(t2) || t1.isEqualTo(t2);
  }
};

} // namespace pl_search

#endif // PL_SEARCH_TERM_HPP
