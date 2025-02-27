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

bool PInt::isLessThan(Term &t) const {
  if (typeid(t) == typeid(PVar))
    return false;
  if (typeid(t) != typeid(PInt)) {
    if (typeid(t) != typeid(PFloat))
      return true;
    PFloat f = static_cast<PFloat &>(t);
    return value < f.getValue();
  }
  PInt i = static_cast<PInt &>(t);
  return value < i.getValue();
}

bool PFloat::isLessThan(Term &t) const {
  if (typeid(t) == typeid(PVar))
    return false;
  if (typeid(t) != typeid(PFloat)) {
    if (typeid(t) != typeid(PInt))
      return true;
    PInt i = static_cast<PInt &>(t);
    return value < i.getValue();
  }
  PFloat f = static_cast<PFloat &>(t);
  return value < f.getValue();
}

bool PAtom::isLessThan(Term &t) const {
  if (typeid(t) == typeid(PVar))
    return false;
  if (typeid(t) == typeid(PInt))
    return false;
  if (typeid(t) == typeid(PFloat))
    return false;
  if (typeid(t) != typeid(PAtom))
    return true;
  PAtom a = static_cast<PAtom &>(t);
  return name < a.getName();
}

bool CList::isLessThan(Term &t) const {
  if (typeid(t) == typeid(PVar))
    return false;
  if (typeid(t) == typeid(PInt))
    return false;
  if (typeid(t) == typeid(PFloat))
    return false;
  if (typeid(t) == typeid(PAtom))
    return false;
  if (typeid(t) != typeid(CList))
    return true;
  CList l = static_cast<CList &>(t);
  return elements < l.elements;
}

} // namespace pl_search