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

#ifndef PL_SEARCH_PINT_HPP
#define PL_SEARCH_PINT_HPP

#include "term.hpp"
#include "typedefs.hpp"

#include <string>
#include <typeinfo>

/**
 * @file pint.hpp
 * @brief Definition of the PInt class.
 */

namespace pl_search {

/**
 * @brief Represents a Prolog integer.
 *
 * PInt objects approximate Prolog integers. Integers are immutable and cannot
 * be bound to other terms.
 */
class PInt : public Term {
public:
  /**
   * @brief Constructs a PInt with the given value.
   * @param value The value of the integer.
   */
  PInt(int value) : value(value) {}

  /**
   * @brief Binds the term to another term.
   * @param t The term to bind to.
   * @return False, as integers cannot be bound to other terms.
   */
  bool bind(TermPtr t) override {
    return false; // Integers cannot be bound to other terms
  }

  /**
   * @brief Resets the term.
   * @param t The term to reset to.
   */
  void reset(TermPtr t) override {
    // No-op for PInt
  }

  /**
   * @brief Returns a string representation of the integer.
   * @return A string representation of the integer.
   */
  std::string repr() const override { return std::to_string(value); }

  /**
   * @brief Checks if the term is equal to another term.
   * @param t The term to compare to.
   * @return True if the terms are equal, false otherwise.
   */
  bool isEqualTo(Term &t) const override {
    PInt *i = dynamic_cast<PInt *>(&t);
    if (i == nullptr)
      return false;
    return value == i->value;
  }

  /**
   * @brief Checks if the term is less than another term.
   * @param t The term to compare to.
   * @return True if the term is less than the other term, false otherwise.
   */
  bool isLessThan(Term &t) const override;

  /**
   * @brief Get the value of the integer.
   * @return The value of the integer.
   */
  int getValue() const { return value; }

private:
  int value; ///< The value of the integer.
};

} // namespace pl_search

#endif // PL_SEARCH_PINT_HPP