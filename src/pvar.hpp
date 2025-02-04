#ifndef PVAR_HPP
#define PVAR_HPP

#include "term.hpp"
#include <string>
#include <cassert>

// PVar objects approximate Prolog variables
class PVar : public Term {
public:
  static int id;

  Term* value;
  Term* dereference() override;
  bool bind(Term* t) override;
  void reset() override;
  
  PVar() {
    value = this;
    var_id = id++;
  }

  int getVarId() const {
    return var_id;
  }

  std::string repr() const override {
    return "X" + std::to_string(var_id);
  }

  bool isEqualTo(Term& t) override {
    PVar* v = dynamic_cast<PVar*>(&t);
    if (v == nullptr) return false;
    return getVarId() == v->getVarId();
  }

  bool isLessThan(Term& t) override;

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
  UpdatablePVar(Term* t) : PVar() {
    value = t;
  }

  // As the intention is to update the value of the variable, we need to
  // simply return this pointer so that a following bind will update the value.
  // If we wanted to do a full dereference for an updatable variable, v, we would
  // use v->PVar::dereference() or v->getValue()->dereference()
  Term* dereference() override {
    return this;
  }

  getValue() {
    return value;
  } 
  
};


#endif // PVAR_HPP
