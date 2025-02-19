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
Term *PVar::dereference() {
  Term *result = this;

  while (true) {
    // Check if the current result is a PVar
    if (PVar *next = dynamic_cast<PVar *>(result)) {
      // If the PVar points to itself, return it
      if (result == next->value)
        return result;
      // Otherwise, follow the chain
      result = next->value;
    } else {
      // If the result is not a PVar, return it
      return result;
    }
  }
  // This should never be reached
  assert(false);
  return result;
}

/**
 * @brief Checks if the term is a variable.
 * @return True if the term is a variable, false otherwise.
 */
bool PVar::is_var() {
  Term *deref = dereference();
  if (PVar *v = dynamic_cast<PVar *>(deref)) {
    return true;
  }
  return false;
}

/**
 * @brief Binds the variable to a term.
 * @param t The term to bind to.
 * @return True if the binding is successful, false otherwise.
 */
bool PVar::bind(Term *t) {
  // Dereference the term to find its actual value
  Term *deref = t->dereference();
  // If the dereferenced term is the same as this variable, return true
  if (this == deref)
    return true;
  // Otherwise, bind this variable to the dereferenced term
  value = deref;
  return true;
}

/**
 * @brief Resets the variable to point at the supplied term.
 * @param t The term to reset to.
 */
void PVar::reset(Term *t) { value = t; }

/**
 * @brief Checks if the variable is less than another term.
 * @param t The term to compare to.
 * @return True if the variable is less than the other term, false otherwise.
 */
bool PVar::isLessThan(Term &t) {
  PVar *v = dynamic_cast<PVar *>(&t);
  if (v == nullptr)
    return true;
  return getVarId() < v->getVarId();
}

} // namespace pl_search