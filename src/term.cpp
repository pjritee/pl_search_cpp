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
 * @brief comparison operators for TermPtrs and the isLessThan method for
 * subclasses of Term.
 */

#include "pl_search/term.hpp"
#include "pl_search/clist.hpp"
#include "pl_search/patom.hpp"
#include "pl_search/pfloat.hpp"
#include "pl_search/pint.hpp"
#include "pl_search/pvar.hpp"

namespace pl_search {

/**
  * @brief == test for TermPtrs
   * @param t1 The first term.
   * @param t2 The second term.
   * @return True if the terms are equal using the isEqualTo method, false
  otherwise.
 */
bool operator==(Term &t1, Term &t2) {
  return t1.deref_term()->isEqualTo(*(t2.deref_term()));
}

/**
   * @brief < operator for Terms.
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
bool operator<(Term &t1, Term &t2) {
  return t1.deref_term()->isLessThan(*(t2.deref_term()));
}

/**
 * @brief <= operator for Terms
 * @param t1 The first term.
 * @param t2 The second term.
 * @return True if the first term is less than or equal to the second term,
 * false otherwise.
 */
bool operator<=(Term &t1, Term &t2) {
  Term *deref1 = t1.deref_term();
  Term *deref2 = t2.deref_term();
  return deref1->isLessThan(*deref2) || deref1->isEqualTo(*deref2);
}

/**
 * @brief < operator for a PInt and a Term
 * @param other The term being compared
 * @return True if the this PInt is < other, false otherwise.
 */
bool PInt::isLessThan(Term &other) const {
  if (dynamic_cast<PVar *>(&other))
    return false;
  if (PInt *i = dynamic_cast<PInt *>(&other)) {
    return value < i->getValue();
  }
  if (PFloat *f = dynamic_cast<PFloat *>(&other)) {

    return value < f->getValue();
  }
  return true;
}

/**
 * @brief < operator for a PFloat and a Term
 * @param other The term being compared
 * @return True if the this PFloat is < other, false otherwise.
 */
bool PFloat::isLessThan(Term &other) const {
  if (dynamic_cast<PVar *>(&other))
    return false;
  if (PInt *i = dynamic_cast<PInt *>(&other)) {
    return value < i->getValue();
  }
  if (PFloat *f = dynamic_cast<PFloat *>(&other)) {
    return value < f->getValue();
  }
  return true;
}

/**
 * @brief < operator for a PAtom and a Term
 * @param other The term being compared
 * @return True if the this PAtom is < other, false otherwise.
 */
bool PAtom::isLessThan(Term &other) const {
  if (dynamic_cast<PVar *>(&other))
    return false;
  if (dynamic_cast<PInt *>(&other))
    return false;
  if (dynamic_cast<PFloat *>(&other))
    return false;
  if (PAtom *f = dynamic_cast<PAtom *>(&other)) {
    return name < f->getName();
  }
  return true;
}

/**
 * @brief < operator for a CList and a Term
 * @param other The term being compared
 * @return True if the this CList is < other, false otherwise.
 */
bool CList::isLessThan(Term &other) const {
  if (dynamic_cast<PVar *>(&other))
    return false;
  if (dynamic_cast<PInt *>(&other))
    return false;
  if (dynamic_cast<PFloat *>(&other))
    return false;
  if (dynamic_cast<PAtom *>(&other))
    return false;
  if (CList *list = dynamic_cast<CList *>(&other)) {
    return elements < list->elements;
  }
  return true;
}

} // namespace pl_search