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

#ifndef PL_SEARCH_PFLOAT_HPP
#define PL_SEARCH_PFLOAT_HPP

#include "term.hpp"
#include "typedefs.hpp"

#include <string>
#include <typeinfo>

/**
 * @file pfloat.hpp
 * @brief Definition of the PFloat class.
 */

namespace pl_search {

/**
 * @brief Represents a Prolog float.
 *
 * PFloat objects approximate Prolog floats. Floats are immutable and cannot be
 * bound to other terms.
 */
class PFloat : public Term {
public:
  /**
   * @brief Constructs a PFloat with the given value.
   * @param value The value of the float.
   */
  PFloat(double value) : value(value) {}


  /**
   * @brief Returns a string representation of the float.
   * @return A string representation of the float.
   */
  std::string repr() const override { return std::to_string(value); }

  /**
   * @brief Checks if the term is equal to another term.
   * @param t The term to compare to.
   * @return True if the terms are equal, false otherwise.
   */
  bool isEqualTo(Term &t) const override {
    PFloat *f = dynamic_cast<PFloat *>(&t);
    if (f == nullptr)
      return false;
    return value == f->value;
  }

  /**
   * @brief < operator for a PFloat and a Term
   * @param other The term being compared
   * @return True if the this PFloat is < other, false otherwise.
   */
  bool isLessThan(Term &t) const override;

  /**
   * @brief Returns the value of the float.
   * @return The value of the float.
   */
  double getValue() const { return value; }

private:
  double value; ///< The value of the float.
};

} // namespace pl_search

#endif // PL_SEARCH_PFLOAT_HPP