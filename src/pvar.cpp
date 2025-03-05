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
 * @file pvar.cpp
 * @brief Implementation of the PVar class.
 */

#include "pl_search/pvar.hpp"
#include <cassert>

namespace pl_search {

// Initialize static member
int PVar::id = 0;

/**
 * @brief Dereferences the variable to find the actual term it points to.
 * @return A pointer to the dereferenced term.
 */
TermPtr PVar::dereference() {
  if (value == nullptr) {
    // an unbound variable
    return shared_from_this();
  }
  PVar *val = this;
  while (PVar *next = dynamic_cast<PVar *>(val->value.get())) {
    // val->value is a variable
    if (next->value == nullptr) {
      // next is an unbound variable
      break;
    }
    val = next;
  }
  // val->value is either a non-var term or an unbound variable
  return val->value;
}

Term *PVar::deref_term() {
  if (value == nullptr) {
    // an unbound variable
    return this;
  }
  PVar *val = this;
  while (PVar *next = dynamic_cast<PVar *>(val->value.get())) {
    // val->value is a variable
    if (next->value == nullptr) {
      // next is an unbound variable
      break;
    }
    val = next;
  }
  // val->value is either a non-var term or an unbound variable
  return val->value.get();
}

/**
 * @brief Checks if the term is a variable.
 * @return True if the term is a variable, false otherwise.
 */
bool PVar::is_var() {
  Term *deref = deref_term();
  PVar *t = dynamic_cast<PVar *>(deref);
  return (t != nullptr);
}

/**
 * @brief Binds the variable to a term.
 * @param t The term to bind to.
 * @return True if the binding is successful, false otherwise.
 * Precondition: t has been dereferenced before the call
 * (bind is only called from unify)
 */
bool PVar::bind(const TermPtr &t) {
  // assert(t == t->dereference());
  if (this == t.get()) {
    // The same variable so nothing to do
    return true;
  }
  // Otherwise, bind this variable to the dereferenced term
  value = t;
  return true;
}

/**
 * @brief Resets the variable to point at the supplied term.
 * @param t The term to reset to.
 */
void PVar::reset(const TermPtr &t) { value = t; }

/**
 * @brief Checks if the variable is less than another term.
 * @param t The term to compare to.
 * @return True if the variable is less than the other term, false otherwise.
 */
bool PVar::isLessThan(Term &t) const {
  PVar *v = dynamic_cast<PVar *>(&t);
  if (v == nullptr)
    return true;
  return getVarId() < v->getVarId();
}

} // namespace pl_search