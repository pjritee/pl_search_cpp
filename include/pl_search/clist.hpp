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

#ifndef PL_SEARCH_CLIST_HPP
#define PL_SEARCH_CLIST_HPP

#include "term.hpp"
#include "typedefs.hpp"

#include <list>
#include <sstream>
#include <string>

/**
 * @file clist.hpp
 * @brief Definition of the CList class.
 */

// CList objects replace Prolog lists for efficiency
// WARNING: Because the internal list is mutable care must be taken when using
// CList objects. For example, it might be necessary to copy the list before
// passing it to a function that might modify it or when binding a variable to
// a CList object.
// If a Prolog-like list is required, a user-defined class that implements the
// Term interface should be used (for example ConsList).

namespace pl_search {

/**
 * @brief Represents a list of terms.
 *
 * CList objects replace Prolog lists for efficiency. The internal list is
 * mutable, so care must be taken when using CList objects. For example, it
 * might be necessary to copy the list before passing it to a function that
 * might modify it or when binding a variable to a CList object.
 */
class CList : public Term {
public:
  /**
   * @brief Default constructor.
   */
  CList() {}

  /**
   * @brief Constructs a CList with the given elements.
   * @param elements The elements of the list.
   */
  CList(const std::list<TermPtr> &elements) : elements(elements) {}

  /**
   * @brief Binds the term to another term.
   * @param t The term to bind to.
   * @return False, as lists cannot be bound to other terms.
   */
  bool bind(TermPtr t) override {
    return false; // Lists cannot be bound to other terms
  }

  /**
   * @brief Resets the term.
   * @param t The term to reset to.
   */
  void reset(TermPtr t) override {
    // No-op for CList
  }

  /**
   * @brief Returns a string representation of the list.
   * @return A string representation of the list.
   */
  std::string repr() const override {
    std::ostringstream oss;
    oss << "[";
    for (auto it = elements.begin(); it != elements.end(); ++it) {
      if (it != elements.begin()) {
        oss << ", ";
      }
      oss << (*it)->repr();
    }
    oss << "]";
    return oss.str();
  }

  /**
   * @brief Checks if the term is equal to another term.
   * @param t The term to compare to.
   * @return True if the terms are equal, false otherwise.
   */
  bool isEqualTo(Term &t) const override {
    if (typeid(*this) != typeid(t)) {
      return false;
    }
    CList l = static_cast<CList &>(t);

    return elements == l.elements;
  }

  /**
   * @brief Checks if the term is less than another term.
   * @param t The term to compare to.
   * @return True if the term is less than the other term, false otherwise.
   */
  bool isLessThan(Term &t) const override;

  /**
   * @brief Adds an element to the list.
   * @param element The element to add.
   */
  void addElement(TermPtr element) { elements.push_back(element); }

  /**
   * @brief Returns the elements of the list.
   * @return A reference to the list of elements.
   */
  const std::list<TermPtr> &getElements() const { return elements; }

private:
  std::list<TermPtr> elements; ///< The elements of the list.
};

} // namespace pl_search

#endif // PL_SEARCH_CLIST_HPP