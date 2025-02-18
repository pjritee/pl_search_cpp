#define CATCH_CONFIG_MAIN
#include "pl_search/choice_iterator.hpp"
#include "pl_search/engine.hpp"
#include "pl_search/pint.hpp"
#include "pl_search/pred.hpp"
#include "pl_search/pvar.hpp"
#include <catch2/catch.hpp>

using namespace pl_search;

// Define a simple predicate for testing
class TestPred : public Pred {
public:
  TestPred(Engine *eng) : Pred(eng) {}

  void initialize_call() override {
    // Initialize the call
  }

  bool apply_choice() override {
    // Apply choice logic
    return true;
  }

  bool test_choice() override {
    // Test choice logic
    return true;
  }

  bool more_choices() override {
    // Determine if there are more choices
    return false;
  }
};

// A predicate that records the current value of a variable and then fails
class CollectAndFail : public SemiDetPred {
public:
  std::list<Term *> &results;
  PVar *var;
  CollectAndFail(Engine *eng, PVar *v, std::list<Term *> &r)
      : SemiDetPred(eng), var(v), results(r) {}

  void initialize_call() override { results.push_back(var->dereference()); }

  bool apply_choice() override { return false; }

  bool test_choice() override { return false; }
};

// Equivalent of Prolog fail
class Fail : public SemiDetPred {
public:
  Fail(Engine *eng) : SemiDetPred(eng) {}

  void initialize_call() override {}

  bool apply_choice() override { return false; }

  bool test_choice() override { return false; }
};

class DetPredTest : public DetPred {
public:
  DetPredTest(Engine *eng, Term *t1, Term *t2)
      : DetPred(eng), term1(t1), term2(t2) {}

  void initialize_call() { engine->unify(term1, term2); }

private:
  Term *term1;
  Term *term2;
};

// Loop body test pred
class BodyPred : public DetPred {
public:
  BodyPred(Engine *eng, Term *t1, Term *t2)
      : DetPred(eng), term1(t1), term2(t2) {}

  void initialize_call() { engine->unify(term1, term2); }

private:
  Term *term1;
  Term *term2;
};

//
// Test Loop
class TestBodyFactory : public LoopBodyFactory {
public:
  std::vector<PVar *> vars;
  std::vector<Term *> values;
  TestBodyFactory(Engine *eng, std::vector<PVar *> &vs,
                  std::vector<Term *> vals)
      : LoopBodyFactory(eng), vars(vs), values(vals) {
    index = 0;
  }

  bool loop_continues() override { return index < vars.size(); }

  PredPtr make_body_pred() override {
    Term *t1 = vars[index];
    Term *t2 = values[index++];
    return std::make_shared<BodyPred>(engine, t1, t2);
  }

private:
  int index;
};

void print_pred_chain(PredPtr pred) {
  while (pred != nullptr) {
    pred = pred->get_continuation();
    std::cerr << " : " << pred;
  }
  std::cerr << std::endl;
}

class EngineTest {
public:
  static void do_push(Engine *engine, PredPtr pred) { engine->push(pred); }
  static void test_trail(Engine *engine, PVar *var) {
    engine->trail(var);
    REQUIRE(engine->trail_stack.size() == 1);
    REQUIRE(engine->trail_stack.top()->var == var);
  }

  static void test_backtrack(Engine *engine, PVar *var) {
    engine->trail(var);
    var->bind(new PInt(42));
    engine->backtrack();
    REQUIRE(engine->trail_stack.size() == 0);
    REQUIRE(var->value == var);
  }

  static void test_push(Engine *engine, PredPtr pred) {
    REQUIRE(engine->env_stack.size() == 1);
    REQUIRE(engine->env_stack.top()->pred == pred);
  }

  static bool test_execute(Engine *engine, PredPtr pred, bool unbind) {
    return engine->execute(pred, unbind);
  }
};

TEST_CASE("Engine private member tests", "[Engine]") {
  Engine engine;
  PVar var;
  PredPtr pred = std::make_shared<TestPred>(&engine);
  EngineTest::do_push(&engine, pred);

  SECTION("Trail test") { EngineTest::test_trail(&engine, &var); }

  SECTION("Backtrack test") { EngineTest::test_backtrack(&engine, &var); }

  SECTION("Push check") { EngineTest::test_push(&engine, pred); }
}

TEST_CASE("Engine execute test", "[Engine]") {
  Engine engine;
  PVar var;
  std::list<Term *> results;
  PredPtr failpred = std::make_shared<CollectAndFail>(&engine, &var, results);
  PInt term1(42);
  PInt term2(43);
  std::vector<Term *> choices = {&term1, &term2};
  VarChoiceIterator choice_iterator1(&engine, &var, choices);
  VarChoiceIterator choice_iterator2(&engine, &var, choices);
  PredPtr choicePred1 =
      std::make_shared<ChoicePred>(&engine, &choice_iterator1);
  PredPtr choicePred2 =
      std::make_shared<ChoicePred>(&engine, &choice_iterator2);
  choicePred2->wrap_with_once();

  PredPtr conjunctionPred1 = conjunction({choicePred1, failpred});
  PredPtr conjunctionPred2 = conjunction({choicePred2, failpred});

  SECTION("Execute test - test once") {
    REQUIRE(choicePred2->get_continuation()->get_continuation() == failpred);
  }
  SECTION("Execute test - simple failure") {
    REQUIRE(!EngineTest::test_execute(&engine, failpred, false));
    REQUIRE(results.size() == 1);
    REQUIRE(results.front() == &var);
  }

  SECTION("Execute test - conjunction and backtrack over choice") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred1, false));
    REQUIRE(results.size() == 2);
    REQUIRE(results.front() == &term1);
    REQUIRE(results.back() == &term2);
  }
  SECTION("Execute test - once, conjunction and backtrack over choice") {
    REQUIRE(var.dereference() == &var);
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred2, false));
    REQUIRE(results.size() == 1);
    REQUIRE(results.front() == &term1);
  }
}

TEST_CASE("Test backtracking over deterministic predicate", "[Engine]") {
  Engine engine;
  PVar var;
  PInt term42(42);
  PredPtr failpred = std::make_shared<Fail>(&engine);
  PredPtr detpred = std::make_shared<DetPredTest>(&engine, &var, &term42);
  PredPtr conjunctionPred = conjunction({detpred, failpred});

  SECTION("Test backtracking over deterministic predicate") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred, false));
    REQUIRE(var.dereference() == &var);
  }
}

TEST_CASE("Engine execute disj test", "[Engine]") {
  Engine engine;
  PVar var;
  std::list<Term *> results;
  PredPtr failpred = std::make_shared<CollectAndFail>(&engine, &var, results);
  PInt term1(42);
  PInt term2(43);
  std::vector<Term *> choices = {&term1, &term2};
  VarChoiceIterator choice_iterator1(&engine, &var, choices);
  VarChoiceIterator choice_iterator2(&engine, &var, choices);
  PredPtr choicePred1 =
      std::make_shared<ChoicePred>(&engine, &choice_iterator1);
  PredPtr choicePred2 =
      std::make_shared<ChoicePred>(&engine, &choice_iterator2);

  std::cout << "failpred " << repr(failpred) << std::endl;
  std::cout << "choicepred1 " << repr(choicePred1) << std::endl;
  std::cout << "choicepred2 " << repr(choicePred2) << std::endl;

  PredPtr disjPred = std::make_shared<DisjPred>(
      &engine, std::vector<PredPtr>{choicePred1, choicePred2});
  std::cout << "disjPred " << repr(disjPred) << std::endl;
  PredPtr conjunctionPred3 = conjunction({disjPred, failpred});
  std::cout << "conjPred3 " << repr(conjunctionPred3) << std::endl;

  SECTION("Execute test - disjunction, backtrack over choice") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred3, false));
    REQUIRE(results.size() == 4);
    std::list<Term *>::iterator it = results.begin();
    REQUIRE(*it == &term1);
    ++it;
    REQUIRE(*it == &term2);
    ++it;
    REQUIRE(*it == &term1);
    ++it;
    REQUIRE(*it == &term2);
  }
}

TEST_CASE("Engine execute notnot predicate - not not call succeeds",
          "[Engine]") {
  Engine engine;
  PVar var;
  std::list<Term *> results;
  PredPtr failpred = std::make_shared<CollectAndFail>(&engine, &var, results);
  PInt term1(42);
  PInt term2(43);
  std::vector<Term *> choices = {&term1, &term2};
  VarChoiceIterator choice_iterator1(&engine, &var, choices);
  PredPtr choicePred1 =
      std::make_shared<ChoicePred>(&engine, &choice_iterator1);
  PredPtr notnot_pred = std::make_shared<NotNot>(&engine, choicePred1);
  PredPtr conjunctionPred = conjunction({notnot_pred, failpred});

  SECTION("Engine execute notnot predicate - execute") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred, false));
    REQUIRE(results.size() == 1);
    REQUIRE(results.front() == &var);
  }
}

TEST_CASE("Engine execute notnot predicate - not not call fails", "[Engine]") {
  Engine engine;
  PVar var;
  std::list<Term *> results;
  PredPtr failpred = std::make_shared<CollectAndFail>(&engine, &var, results);
  // PInt term1(42);
  // PInt term2(43);
  // std::vector<Term*> choices = {&term1, &term2};
  // VarChoiceIterator choice_iterator1(&engine, &var, choices);
  PredPtr fail = std::make_shared<Fail>(&engine);
  PredPtr notnotPred = std::make_shared<NotNot>(&engine, fail);
  PredPtr conjunctionPred = conjunction({notnotPred, failpred});

  SECTION("Engine execute notnot predicate - execute") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred, false));
    REQUIRE(results.size() == 0);
  }
}

TEST_CASE("Test loop predicate", "[Engine]") {
  Engine engine;
  PVar v1, v2, v3, v4;
  PInt i1(1), i2(2), i3(3), i4(4);

  std::vector<PVar *> vars = {&v1, &v2, &v3};
  std::vector<Term *> values = {&i1, &i2, &i3};
  PredPtr detpred = std::make_shared<DetPredTest>(&engine, &v4, &i4);
  TestBodyFactory body_factory = TestBodyFactory(&engine, vars, values);
  PredPtr loop = std::make_shared<Loop>(&engine, &body_factory);
  PredPtr conjunctionPred = conjunction({loop, detpred});

  SECTION("Engine execute loop") {
    REQUIRE(EngineTest::test_execute(&engine, conjunctionPred, false));
    REQUIRE(v1.dereference() == &i1);
    REQUIRE(v2.dereference() == &i2);
    REQUIRE(v3.dereference() == &i3);
    REQUIRE(v4.dereference() == &i4);
  }
}
