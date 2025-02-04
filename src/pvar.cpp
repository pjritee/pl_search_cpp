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
#include "pl_search/pvar.hpp"
#include <cassert>

// Initialize static member
int PVar::id = 0;

// Dereference the variable to find the actual term it points to
Term* PVar::dereference() {
  Term* result = this;
  
  while (true) {
    // Check if the current result is a PVar
    if (PVar* next = dynamic_cast<PVar*>(result)) {
      // If the PVar points to itself, return it
      if (result == next->value) return result;
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

// Bind the variable to a term
bool PVar::bind(Term* t) {
  // Dereference the term to find its actual value
  Term* deref = t->dereference();
  // If the dereferenced term is the same as this variable, return true
  if (this == deref) return true;
  // Otherwise, bind this variable to the dereferenced term
  value = deref;
  return true;
}

// Reset the variable to point at supplied term
void PVar::reset(Term* t) {
  value = t;
}

// Define the isLessThan method
bool PVar::isLessThan(Term& t) {
  PVar* v = dynamic_cast<PVar*>(&t);
  if (v == nullptr) return true;
  return getVarId() < v->getVarId();
}