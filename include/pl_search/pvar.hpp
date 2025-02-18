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
#include <cassert>
#include <string>

namespace pl_search {

// PVar objects approximate Prolog variables
class PVar : public Term {
public:
  static int id;

  Term *value;
  Term *dereference() override;
  bool bind(Term *t) override;
  void reset(Term *t) override;

  PVar() {
    value = this;
    var_id = id++;
  }

  int getVarId() const { return var_id; }

  std::string repr() const override { return "X" + std::to_string(var_id); }

  bool isEqualTo(Term &t) override {
    PVar *v = dynamic_cast<PVar *>(&t);
    if (v == nullptr)
      return false;
    return getVarId() == v->getVarId();
  }

  bool isLessThan(Term &t) override;

  bool is_var() override;

private:
  int var_id;
};

// UpdatableVar is used to implement what some Prologs call
// updatable assignment. This is typically used to store (part of) the
// state in a way that can be backtracked over. For example, after
// binding a variable and then making some deductions the available choices
// for another variable might have been reduced and we can use an UpdatableVar
// to store that value as the computation moves forward but on backtracking
// the old value will be restored
class UpdatablePVar : public PVar {
public:
  UpdatablePVar(Term *t) : PVar() { value = t; }

  // As the intention is to update the value of the variable, we need to
  // simply return this pointer so that a following bind will update the value.
  // If we wanted to do a full dereference for an updatable variable, v, we
  // would use v->PVar::dereference() or v->getValue()->dereference()
  Term *dereference() override { return this; }
};

} // namespace pl_search

#endif // PL_SEARCH_PVAR_HPP
