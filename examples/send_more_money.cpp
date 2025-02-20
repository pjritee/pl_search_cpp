/*
MIT License

...license text...

*/

// This example is a demonstration of the pl_search library.
// It is an implementation of a solver for the SEND+MORE=MONEY puzzle.
//
// The puzzle is to choose distinct values for D,E,N,R,S,M,O,Y from
// 0,1,2,3,4,5,6,7,8,9 to make the following addition true.
//
//   SEND
// + MORE
// ------
//  MONEY

#include "pl_search/engine.hpp"
#include "pl_search/pint.hpp"
#include "pl_search/pred.hpp"
#include "pl_search/pvar.hpp"
#include <cassert>
#include <iostream>

using namespace pl_search;

// For this solver we will use the following PVars:
// D,E,N,R,S,M,O,Y
// and the following constraints:
//  The values of S and M are in the range 1-9
//  The values of D,E,N,R,O,Y are in the range 0-9
//  D,E,N,R,S,M,O,Y are distinct
//
// The intention is to bind the variables to integer terms representing the
// digits. To support this we list the digit terms as follows - at index i is
// the PVar for the digit i.
std::vector<PInt> digits = {PInt(0), PInt(1), PInt(2), PInt(3), PInt(4),
                            PInt(5), PInt(6), PInt(7), PInt(8), PInt(9)};

//
// The approach we take to manage these constrains is to define a subclass
// of PVar that will manage the constraints for each variable as follows.

class SendMoreMoneyVar : public PVar {
public:
  SendMoreMoneyVar(int low, int high)
      : PVar(), lower_bound(low), upper_bound(high) {}

  void set_distinct(std::vector<SendMoreMoneyVar *> &d) { distinct = d; }

  bool bind(Term *t) override {
    std::cout << "Binding " << *this << " to " << *t << std::endl;
    if (PInt *p = dynamic_cast<PInt *>(t)) {
      if (p->getValue() >= lower_bound && p->getValue() <= upper_bound) {
        if (distinct.empty()) {
          std::cout << "empty distinct set" << std::endl;
          return PVar::bind(t);
        }
        for (auto &v : distinct) {
          if (*v == *t) {
            return false;
          }
        }
        return PVar::bind(t);
      }
    }
    return false;
  }

private:
  int lower_bound;
  int upper_bound;
  std::vector<SendMoreMoneyVar *> distinct;
};

// We will break the sum constraint into constraints about the individual
// column sums. To do this we will introduce a carry variable for each column.
//  D + E = Y + 10*C1
//  N + R + C1 = E + 10*C2
//  E + O + C2 = N + 10*C3
//  S + M + C3 = O + 10*M
//
// In order to manage these constraints we will define a constraint class
// that will manage the constraints for each column sum.

// For example, the constraint for the column sum D + E = Y + 10*C1
// would be defined as ColumnSumConstraint(engine,{D,E},Y,C1) and
// N + R + C1 = E + 10*C2 would be defined as
// ColumnSumConstraint(engine,{N,R,C1},E,C2)
//
// NOTE: As the try_method is quite complicated it would be better to define
// this class in a separate file and create unit tests for it.
//
class ColumnSumConstraint {
public:
  ColumnSumConstraint(Engine *eng, std::vector<SendMoreMoneyVar *> &lhs,
                      SendMoreMoneyVar *rhs, SendMoreMoneyVar *carry)
      : engine(eng), lhs(lhs), rhs(rhs), carry(carry) {}

  // The try_solve method will be called by the engine to attempt to
  // solve the constraint. If the constraint can't be solved it should
  // return false. If the constraint is already solved it should return
  // true and progress should be left unchanged. If the constraint is
  // not yet solved but progress has been made by binding  variables
  // it should return true and progress should be set to true.
  bool try_solve(bool &progress) {
    for (auto &v : lhs) {
      std::cout << *(v->dereference()) << " ";
    }
    std::cout << std::endl;
    int sum = 0;
    int num_vars_lhs = 0;
    for (auto &v : lhs) {
      if (PInt *p = dynamic_cast<PInt *>(v->dereference())) {
        sum += p->getValue();
      } else {
        num_vars_lhs++;
      }
    }

    if (num_vars_lhs == 0) {
      // above the line is completely determined and so the digit below the
      // line and the carry are also determined
      if (carry->is_var() || rhs->is_var()) {
        progress = true;
      }
      if (!engine->unify(carry, &digits[sum / 10])) {
        return false;
      }
      if (!engine->unify(rhs, &digits[sum % 10])) {
        return false;
      }
      return true;
    }
    if (num_vars_lhs == 1) {
      // all but one of the digits above the line, the digit below the line and
      // the carry are determined and so the remaining digit above the line can
      // be determined
      //
      if (carry->is_var() || rhs->is_var()) {
        return true;
      }
      // both carry and rhs are bound to PInts
      PInt *rhs_deref = dynamic_cast<PInt *>(rhs->dereference());
      PInt *carry_deref = dynamic_cast<PInt *>(carry->dereference());
      assert(rhs_deref != nullptr);
      assert(carry_deref != nullptr);
      int value = rhs_deref->getValue() + 10 * carry_deref->getValue() - sum;
      // value is now the value of the remaining unbound variable above the line
      // Find that variable and bind it
      PVar *var = nullptr;
      for (auto &v : lhs) {
        if (v->is_var()) {
          var = v;
          break;
        }
      }
      if ((value < 0) || (value > 9)) {
        return false;
      }
      assert(var != nullptr);
      if (!engine->unify(var, &digits[value])) {
        return false;
      }
      return true;
    }
    return true;
  }

private:
  Engine *engine;
  std::vector<SendMoreMoneyVar *> lhs;
  SendMoreMoneyVar *rhs;
  SendMoreMoneyVar *carry;
};

// The SendMoreMoneyVars
SendMoreMoneyVar D(0, 9);
SendMoreMoneyVar E(0, 9);
SendMoreMoneyVar N(0, 9);
SendMoreMoneyVar R(0, 9);
SendMoreMoneyVar S(1, 9);
SendMoreMoneyVar M(1, 9);
SendMoreMoneyVar O(0, 9);
SendMoreMoneyVar Y(0, 9);
SendMoreMoneyVar C1(0, 1);
SendMoreMoneyVar C2(0, 1);
SendMoreMoneyVar C3(0, 1);

// The distinctness set for the variables
std::vector<SendMoreMoneyVar *> distinctvars = {&D, &E, &N, &R, &S, &M, &O, &Y};

int main() {
  Engine engine;

  // Set the distinctness set for each variable
  D.set_distinct(distinctvars);
  E.set_distinct(distinctvars);
  N.set_distinct(distinctvars);
  R.set_distinct(distinctvars);
  S.set_distinct(distinctvars);
  M.set_distinct(distinctvars);
  O.set_distinct(distinctvars);
  Y.set_distinct(distinctvars);

  // Above the line vectors
  std::vector<SendMoreMoneyVar *> D_E = {&D, &E};
  std::vector<SendMoreMoneyVar *> N_R_C1 = {&N, &R, &C1};
  std::vector<SendMoreMoneyVar *> E_O_C2 = {&E, &O, &C2};
  std::vector<SendMoreMoneyVar *> S_M_C3 = {&S, &M, &C3};
  // Set the column sum constraints
  ColumnSumConstraint c1(&engine, D_E, &Y, &C1);
  ColumnSumConstraint c2(&engine, N_R_C1, &E, &C2);
  ColumnSumConstraint c3(&engine, E_O_C2, &N, &C3);
  ColumnSumConstraint c4(&engine, S_M_C3, &O, &M);

  // Note that because M is really a carry (and is not 0) then we can
  // set it to 1 and not have to worry about it again
  engine.unify(&M, &digits[1]);

  // test
  engine.unify(&D, &digits[7]);
  // engine.unify(&E, &digits[2]);
  engine.unify(&Y, &digits[2]);
  engine.unify(&C1, &digits[1]);
  bool progress = false;
  if (!c1.try_solve(progress)) {
    std::cout << "Failed to solve c1" << std::endl;
    return 1;
  } else if (progress) {
    std::cout << "Progress made on c1" << std::endl;
  }
  std::cout << "D = " << *(D.dereference()) << std::endl;
  std::cout << "E = " << *(E.dereference()) << std::endl;
  std::cout << "Y = " << *(Y.dereference()) << std::endl;
  std::cout << "C1 = " << *(C1.dereference()) << std::endl;
  return 0;
}