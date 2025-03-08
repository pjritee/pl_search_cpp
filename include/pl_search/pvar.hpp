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

#ifndef PL_SEARCH_PVAR_HPP
#define PL_SEARCH_PVAR_HPP

#include "term.hpp"
#include "typedefs.hpp"

/**
 * @file pvar.hpp
 * @brief Definition of the PVar class.
 */

namespace pl_search {

/**
 * @brief Represents a Prolog variable.
 *
 * PVar objects approximate Prolog variables. Variables can be bound to other
 * terms. An unbound variable is represented by a PVar object with a null value.
 */
class PVar : public Term {

public:
  static int id; ///< Static member to generate unique IDs for variables.

  TermPtr value; ///< The value of the variable.

  /**
   * @brief Constructs a PVar.
   */
  PVar() : value(nullptr), var_id(id++) {}

  /**
   * @brief Dereferences the variable to find the actual term it points to.
   * @return A pointer to the dereferenced term.
   */
  TermPtr dereference() override;

  /**
   * @brief Binds the variable to a term.
   * @param t The term to bind to.
   * @return True if the binding is successful, false otherwise.
   * Precondition: t has been dereferenced before the call
   * (bind is only called from unify)
   */
  virtual bool bind(const TermPtr &t);

  /**
   * @brief Resets the variable to point at the supplied term.
   * @param t The term to reset to.
   */
  void reset(const TermPtr &t);

  /**
   * @brief Checks if the term is a variable.
   * @return True if the term is a variable, false otherwise.
   */
  bool is_var() override;

  /**
   * @brief Checks if the variable is less than another term.
   * @param t The term to compare to.
   * @return True if the variable is less than the other term, false otherwise.
   */
  bool isLessThan(Term &other) const override;

  /**
   * @brief Returns the variable ID.
   * @return The variable ID.
   */
  int getVarId() const { return var_id; }

  std::string repr() const override { return "X" + std::to_string(var_id); }

protected:
  virtual bool isEqualTo(Term &other) const override {
    if (PVar *v = dynamic_cast<PVar *>(&other)) {

      return getVarId() == v->getVarId();
    }
    return false;
  }

  Term *deref_term() override;

private:
  int var_id; ///< The ID of the variable.
};

/**
 * @brief UpdatableVar implements what some Prologs call updatable assignment.
 */
/*
 * This is typically used to store (part of) the
 * state in a way that can be backtracked over. For example, after
 * binding a variable and then making some deductions the available choices
 * for another variable might have been reduced and we can use an UpdatableVar
 * to store that value as the computation moves forward but on backtracking
 * the old value will be restored.
 */
class UpdatablePVar : public PVar {

public:
  UpdatablePVar(TermPtr t) : PVar() { value = t; }

  // As the intention is to update the value of the variable, we need to
  // simply return this pointer so that a following bind will update the value.
  // If we wanted to do a full dereference for an updatable variable, v, we
  // would use v->PVar::dereference() or v->getValue()->dereference()
  TermPtr dereference() override { return shared_from_this(); }

protected:
  Term *deref_term() override { return this; }
};

} // namespace pl_search

#endif // PL_SEARCH_PVAR_HPP
