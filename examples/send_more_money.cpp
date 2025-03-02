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
//

#include "pl_search/choice_iterator.hpp"
#include "pl_search/engine.hpp"
#include "pl_search/patom.hpp"
#include "pl_search/pint.hpp"
#include "pl_search/pred.hpp"
#include "pl_search/pvar.hpp"
#include "pl_search/typedefs.hpp"
#include <cassert>
#include <iostream>
#include <vector>

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
//
// Note that the pl_search library uses smart pointers for both predicates
// and terms. typedefs.hpp defines the shared pointer types for the library
// and macros for creating shared pointers to terms.
// For example NEW_PVAR is a macro that creates a shared pointer to a PVar
// and PVarPtr is a typedef for a shared pointer to a PVar.

std::vector<PIntPtr> digits = {
    NEW_PINT(0), NEW_PINT(1), NEW_PINT(2), NEW_PINT(3), NEW_PINT(4),
    NEW_PINT(5), NEW_PINT(6), NEW_PINT(7), NEW_PINT(8), NEW_PINT(9)};

//
// The approach we take to manage these constrains is to define a subclass
// of PVar that will manage the constraints for each variable as follows.

// Add a typedef and macro for the PuzzleVar class to be consistent with the
// other typedefs and macros in the library.
class PuzzleVar;
typedef std::shared_ptr<PuzzleVar> PuzzleVarPtr;
#define NEW_PUZZLE_VAR std::make_shared<PuzzleVar>

class PuzzleVar : public PVar {
public:
  // The PuzzleVar class will be a subclass of PVar and will manage the
  // constraints for each variable. lower_bound and upper_bound are the
  // range of values for the variable. name is the name of the variable
  // (used for debugging).
  PuzzleVar(int low, int high, std::string name)
      : PVar(), lower_bound(low), upper_bound(high), name(name) {}

  // The set_distinct method will set the distinctness set for the variable.
  // This can't be part of the constructor because the distinctness set is
  // not known until all the variables have been created.
  void set_distinct(std::vector<PuzzleVarPtr> &d) { distinct = d; }

  // The bind method is overridden to check that the value is in the
  // range and that it is distinct from the other variables in the
  // distinctness set.
  bool bind(TermPtr t) override {
    if (PIntPtr p = std::dynamic_pointer_cast<PInt>(t)) {
      if (p->getValue() >= lower_bound && p->getValue() <= upper_bound) {
        if (distinct.empty()) {
          return PVar::bind(t);
        }
        for (auto v : distinct) {
          if (v->dereference() == t) {
            return false;
          }
        }
        return PVar::bind(t);
      }
    }
    return false;
  }

  // The get_choices method is overridden to return the choices for the
  // variable. The choices are the digits in the range that are not in the
  // distinctness set.
  std::vector<TermPtr> get_choices() {
    std::vector<TermPtr> choices;
    for (int i = lower_bound; i <= upper_bound; i++) {
      PIntPtr d = digits[i];
      bool in_distinct = false;
      for (auto v : distinct) {
        if (v->dereference() == d) {
          in_distinct = true;
          break;
        }
      }
      // If the digit is not in the distinctness set then add it to the
      // choices.
      if (!in_distinct) {
        choices.push_back(d);
      }
    }
    return choices;
  }

  // Override for better debugging
  std::string repr() const override { return name; }

private:
  int lower_bound;
  int upper_bound;
  std::string name;
  std::vector<PuzzleVarPtr> distinct;
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
// this class in a separate file and create unit tests for it. The same comment
// applies to the other classes.
//

class ColumnSumConstraint {

public:
  // The solved_value is a static variable that is used to indicate that the
  // constraint has been solved by binding the solved variable to this value.
  // The actual value is not important as long as it is not a variable.
  // A constraint will become solved because of choices made for variables
  // since a choicepoint. When backtracking over the choicepoint the constraint
  // will be reset to unsolved because the solved variable will be unbound.
  static PAtomPtr solved_value;

  ColumnSumConstraint(Engine *eng, std::vector<PuzzleVarPtr> &lhs,
                      PuzzleVarPtr rhs, PuzzleVarPtr carry)
      : engine(eng), lhs(lhs), rhs(rhs), carry(carry) {
    solved = NEW_PVAR();
  }

  // The try_solve method will be called indirectly by the test_choice method
  // of the choice iterator below. If the constraint can't be solved it should
  // return false. If the constraint is already solved it should return
  // true and progress should be left unchanged. If the constraint is
  // not yet solved but progress has been made by binding  variables
  // it should return true and progress should be set to true.
  bool try_solve(bool &progress) {
    // If the constraint is already solved then return true
    if (!solved->is_var()) {
      return true;
    }
    // We start by considering lhs (the above-the-line digits).
    // sum is the sum of the known digits above the line and num_vars_lhs
    // is the number of remaining variables above the line.
    int sum = 0;
    int num_vars_lhs = 0;
    for (auto &v : lhs) {
      if (PIntPtr p = std::dynamic_pointer_cast<PInt>(v->dereference())) {
        // If the variable is bound then add its value to the sum
        sum += p->getValue();
      } else {
        // If the variable is not bound then increment the number of
        // variables above the line.
        num_vars_lhs++;
      }
    }

    if (num_vars_lhs == 0) {
      // above the line is completely determined and so the digit below the
      // line and the carry can also be determined
      if (carry->is_var() || rhs->is_var()) {
        // If the carry or the digit below the line is a variable then
        // we can determine their values and so progress has been made.
        progress = true;
      }
      // Note that if either carry or rhs are already digits then
      // unify will succeed if and only if they are the correct values.
      if (!engine->unify(carry, digits[sum / 10])) {
        return false;
      }
      if (!engine->unify(rhs, digits[sum % 10])) {
        return false;
      }
      // The constraint is now solved so bind the solved variable to the
      // solved_value.
      engine->unify(solved, solved_value);
      return true;
    }
    if (num_vars_lhs == 1) {
      // All but one of the digits above the line is determined.
      // If either the carry or the digit below the line is a variable
      // then we can't make progress
      if (carry->is_var() || rhs->is_var()) {
        return true;
      }
      // Both carry and rhs are digits so we can determine the value of the
      // remaining variable above the line.
      PIntPtr rhs_deref = std::dynamic_pointer_cast<PInt>(rhs->dereference());
      PIntPtr carry_deref =
          std::dynamic_pointer_cast<PInt>(carry->dereference());
      assert(rhs_deref);
      assert(carry_deref);
      int value = rhs_deref->getValue() + 10 * carry_deref->getValue() - sum;
      // value is now the value of the remaining unbound variable above the line
      // Find that variable and bind it.
      TermPtr var;
      bool found = false;
      for (TermPtr v : lhs) {
        if (v->is_var()) {
          var = v;
          found = true;
          break;
        }
      }
      assert(found);
      if ((value < 0) || (value > 9)) {
        // The value is out of range so the constraint can't be solved
        return false;
      }
      if (!engine->unify(var, digits[value])) {
        // The value can't be bound so the constraint can't be solved.
        // Note that unify will fail because bind will fail and that is
        // because the value is in the distinctness set.
        return false;
      }
      // The constraint is now solved so bind the solved variable to the
      // solved_value.
      engine->unify(solved, solved_value);
      return true;
    }
    // If we get here then there are more than one variable above the line
    // that are not bound. We can't make progress so return true.
    return true;
  }

private:
  Engine *engine;
  std::vector<PuzzleVarPtr> &lhs;
  PuzzleVarPtr rhs;
  PuzzleVarPtr carry;
  PVarPtr solved;
};

// The solved_value is a static variable that is used to indicate that the
// constraint has been solved by binding the solved variable to this value.
PAtomPtr ColumnSumConstraint::solved_value = NEW_PATOM("solved");

// The AllConstraints class will manage all the constraints for the puzzle.
class AllConstraints {
public:
  // We pass in the puzzle variables so that we can iterate over them
  // to find variables that have fewer than two choices.
  AllConstraints(Engine *eng, std::vector<PuzzleVarPtr> &vars)
      : engine(eng), vars(vars) {}

  void add_constraint(ColumnSumConstraint *c) { constraints.push_back(c); }

  // The try_solve method will try to solve all the constraints. If it can't
  // solve a constraint then it will return false otherwise it will return true.
  bool try_solve() {
    bool progress = true;
    // We keep trying to solve the constraints until we can't make any progress.
    while (progress) {
      progress = false;
      // Try to solve each constraint in turn.
      for (auto &c : constraints) {
        if (!c->try_solve(progress)) {
          return false;
        }
      }
      // Find any variables that have fewer than two choices.
      for (auto &v : vars) {
        if (v->is_var()) {
          std::vector<TermPtr> choices = v->get_choices();
          // If there are no choices then the puzzle can't be solved (with these
          // constraints).
          if (choices.empty()) {
            return false;
          }
          if (choices.size() == 1) {
            // If there is only one choice then bind the variable to that value.
            if (!engine->unify(v, choices[0])) {
              return false;
            }
            progress = true;
          }
        }
      }
    }
    // None of the constraints have faild and we have made as much progress as
    // we can so return true.
    return true;
  }

private:
  Engine *engine;
  std::vector<ColumnSumConstraint *> constraints;
  std::vector<PuzzleVarPtr> &vars;
};

// The PrintAndFail class is a predicate that will print the solution
// and then fail. This will case the engine to backtrack to find all
// solutions.
class PrintAndFail : public SemiDetPred {
public:
  PrintAndFail(Engine *eng, std::vector<PuzzleVarPtr> &vars)
      : SemiDetPred(eng), vars(vars) {}

  void initialize_call() override {
    // Pretty print the solution - we are relying on the order of variables
    // (now digits) in vars
    std::cout << " " << *(vars[0]->dereference()) << *(vars[1]->dereference())
              << *(vars[2]->dereference()) << *(vars[3]->dereference())
              << std::endl;
    std::cout << "+" << std::endl;
    std::cout << " " << *(vars[4]->dereference()) << *(vars[5]->dereference())
              << *(vars[6]->dereference()) << *(vars[1]->dereference())
              << std::endl;
    std::cout << "-----" << std::endl;
    std::cout << *(vars[4]->dereference()) << *(vars[5]->dereference())
              << *(vars[2]->dereference()) << *(vars[1]->dereference())
              << *(vars[7]->dereference()) << std::endl
              << std::endl;
  }

private:
  std::vector<PuzzleVarPtr> &vars;
};

// PuzzleChoiceIterator is a subclass of VarChoiceIterator that overrides
// the test_choice method so that we can attempt to solve the constraints
// based on the choice for the variable.
class PuzzleChoiceIterator : public VarChoiceIterator {
public:
  PuzzleChoiceIterator(Engine *engine, PuzzleVarPtr v, std::vector<TermPtr> ch,
                       AllConstraints *constraints)
      : VarChoiceIterator(engine, v, ch), constraints(constraints) {}

  bool test_choice() override { return constraints->try_solve(); }

private:
  AllConstraints *constraints;
};
// To solve the puzzle we need to choose values for several variables and
// that is done by using the Loop predicate whic is drived by the following
// factory.
class PuzzleLoopBodyFactory : public LoopBodyFactory {
public:
  PuzzleLoopBodyFactory(Engine *eng, std::vector<PuzzleVarPtr> *vs,
                        AllConstraints *constraints)
      : LoopBodyFactory(eng), vars(vs), constraints(constraints) {}

  // The loop should continue if there are any unbound puzzle variables.
  // If it continues then next_var will be set to one of these variables.
  bool loop_continues() override {
    for (auto v : *vars) {
      if (v->is_var()) {
        next_var = v;
        return true;
      }
    }
    return false;
  }

  // The body predicate is a ChoicePred  whose iterator is
  // based on next_var and it's possible choices.
  PredPtr make_body_pred() override {
    choice_iterator = std::make_shared<PuzzleChoiceIterator>(
        engine, next_var, next_var->get_choices(), constraints);
    return std::make_shared<ChoicePred>(engine, choice_iterator);
  }

private:
  std::vector<PuzzleVarPtr> *vars;
  AllConstraints *constraints;
  PuzzleVarPtr next_var;
  ChoiceIteratorPtr choice_iterator;
};

int main() {
  std::cout << std::endl
            << "Solutions of the SEND+MORE=MONEY puzzle: " << std::endl
            << std::endl;

  Engine engine;
  // The PuzzleVars - pass in the variable name for debugging
  PuzzleVarPtr S = NEW_PUZZLE_VAR(1, 9, "S");
  PuzzleVarPtr E = NEW_PUZZLE_VAR(0, 9, "E");
  PuzzleVarPtr N = NEW_PUZZLE_VAR(0, 9, "N");
  PuzzleVarPtr D = NEW_PUZZLE_VAR(0, 9, "D");
  PuzzleVarPtr M = NEW_PUZZLE_VAR(1, 9, "M");
  PuzzleVarPtr O = NEW_PUZZLE_VAR(0, 9, "O");
  PuzzleVarPtr R = NEW_PUZZLE_VAR(0, 9, "R");
  PuzzleVarPtr Y = NEW_PUZZLE_VAR(0, 9, "Y");
  PuzzleVarPtr C1 = NEW_PUZZLE_VAR(0, 1, "C1");
  PuzzleVarPtr C2 = NEW_PUZZLE_VAR(0, 1, "C2");
  PuzzleVarPtr C3 = NEW_PUZZLE_VAR(0, 1, "C3");

  // The distinctness set for the variables
  std::vector<PuzzleVarPtr> distinctvars = {S, E, N, D, M, O, R, Y};
  // All the puzzle variables.
  std::vector<PuzzleVarPtr> allvars = {S, E, N, D, M, O, R, Y, C1, C2, C3};

  //  Set the distinctness set for each variable
  S->set_distinct(distinctvars);
  E->set_distinct(distinctvars);
  N->set_distinct(distinctvars);
  D->set_distinct(distinctvars);
  M->set_distinct(distinctvars);
  O->set_distinct(distinctvars);
  R->set_distinct(distinctvars);
  Y->set_distinct(distinctvars);

  // Above the line vectors
  std::vector<PuzzleVarPtr> D_E = {D, E};
  std::vector<PuzzleVarPtr> N_R_C1 = {N, R, C1};
  std::vector<PuzzleVarPtr> E_O_C2 = {E, O, C2};
  std::vector<PuzzleVarPtr> S_M_C3 = {S, M, C3};
  // Set the column sum constraints
  ColumnSumConstraint c1(&engine, D_E, Y, C1);
  ColumnSumConstraint c2(&engine, N_R_C1, E, C2);
  ColumnSumConstraint c3(&engine, E_O_C2, N, C3);
  ColumnSumConstraint c4(&engine, S_M_C3, O, M);

  AllConstraints all_constraints(&engine, allvars);
  all_constraints.add_constraint(&c1);
  all_constraints.add_constraint(&c2);
  all_constraints.add_constraint(&c3);
  all_constraints.add_constraint(&c4);

  // Note that because M is really a carry (and is not 0) then we can
  // set it to 1 and not have to worry about it again
  engine.unify(M, digits[1]);

  // It migth be possible that some of the constraints can be solved
  // up front
  if (!all_constraints.try_solve()) {
    return 0;
  }

  // Create the loop predicate
  PuzzleLoopBodyFactory loop_body_factory(&engine, &allvars, &all_constraints);
  PredPtr loop = std::make_shared<Loop>(&engine, &loop_body_factory);

  PredPtr print_and_fail = std::make_shared<PrintAndFail>(&engine, allvars);

  // The complete solver is a conjunction of the loop predicate (which finds
  // all possible solutions on backtracking) and the print_and_fail predicate
  // that pretty-prints a solution and then fails (to trigger backtracking).
  PredPtr conjunction_pred = conjunction({loop, print_and_fail});
  engine.execute(conjunction_pred, false);
  std::cout << std::endl << std::endl << "End of solutions" << std::endl;
  return 0;
}
