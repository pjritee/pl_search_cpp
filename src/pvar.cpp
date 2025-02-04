#include "pvar.hpp"
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

// Reset the variable to point to itself
void PVar::reset() {
  value = this;
}

// Define the isLessThan method
bool PVar::isLessThan(Term& t) {
  PVar* v = dynamic_cast<PVar*>(&t);
  if (v == nullptr) return true;
  return getVarId() < v->getVarId();
}