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

#ifndef PL_SEARCH_PATOM_HPP
#define PL_SEARCH_PATOM_HPP

#include "term.hpp"
#include "typedefs.hpp"

#include <string>
#include <typeinfo>

/**
 * @file patom.hpp
 * @brief Definition of the PAtom class.
 */

namespace pl_search {

/**
 * @brief Represents a Prolog atom.
 *
 * PAtom objects approximate Prolog atoms. Atoms are immutable and cannot be
 * bound to other terms.
 */
class PAtom : public Term {
public:
  /**
   * @brief Constructs a PAtom with the given name.
   * @param name The name of the atom.
   */
  PAtom(const std::string &name) : name(name) {}

  /**
   * @brief Binds the term to another term.
   * @param t The term to bind to.
   * @return False, as atoms cannot be bound to other terms.
   */
  bool bind(TermPtr t) override {
    return false; // Atoms cannot be bound to other terms
  }



  /**
   * @brief Returns the name of the atom.
   * @return The name of the atom.
   */
  std::string getName() const { return name; }

  /**
   * @brief Returns a string representation of the atom.
   * @return A string representation of the atom.
   */
  std::string repr() const override { return name; }

  /**
   * @brief Checks if the term is equal to another term.
   * @param t The term to compare to.
   * @return True if the terms are equal, false otherwise.
   */
  bool isEqualTo(Term &t) const override {
    PAtom *a = dynamic_cast<PAtom *>(&t);
    if (a == nullptr)
      return false;
    return name == a->name;
  }

  /**
   * @brief < operator for a PAtom and a Term
   * @param other The term being compared
   * @return True if the this PAtom is < other, false otherwise.
   */
  bool isLessThan(Term &t) const override;

private:
  std::string name; ///< The name of the atom.
};

} // namespace pl_search

#endif // PL_SEARCH_PATOM_HPP