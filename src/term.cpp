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
 * @file term.cpp
 * @brief helper functions for term.hpp
 */

#include "pl_search/term.hpp"

namespace pl_search {

/**
  * @brief == test for TermPtrs
   * @param t1 The first term.
   * @param t2 The second term.
   * @return True if the terms are equal using the isEqualTo method, false
  otherwise.
 */
bool operator==(TermPtr t1, TermPtr t2) {
  return t1->dereference()->isEqualTo(*(t2->dereference()));
}

/**
   * @brief < operator for TermPtrs.
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
bool operator<(TermPtr t1, TermPtr t2) {
  return t1->dereference()->isLessThan(*(t2->dereference()));
}

/**
 * @brief <= operator for TermPtrs
 * @param t1 The first term.
 * @param t2 The second term.
 * @return True if the first term is less than or equal to the second term,
 * false otherwise.
 */
bool operator<=(TermPtr t1, TermPtr t2) {
  TermPtr deref1 = t1->dereference();
  TermPtr deref2 = t2->dereference();
  return deref1->isLessThan(*deref2) || deref1->isEqualTo(*deref2);
}

} // namespace pl_search