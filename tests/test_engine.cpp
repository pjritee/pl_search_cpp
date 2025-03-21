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

  bool more_choices() override {
    // Determine if there are more choices
    return false;
  }
};

// A predicate that records the current value of a variable and then fails
class CollectAndFail : public SemiDetPred {
public:
  std::list<TermPtr> &results;
  PVarPtr var;
  CollectAndFail(Engine *eng, PVarPtr v, std::list<TermPtr> &r)
      : SemiDetPred(eng), var(v), results(r) {}

  void initialize_call() override { results.push_back(var->dereference()); }

  bool apply_choice() override { return false; }
};

// Equivalent of Prolog fail
class Fail : public SemiDetPred {
public:
  Fail(Engine *eng) : SemiDetPred(eng) {}

  void initialize_call() override {}

  bool apply_choice() override { return false; }
};

class DetPredTest : public DetPred {
public:
  DetPredTest(Engine *eng, TermPtr t1, TermPtr t2)
      : DetPred(eng), term1(t1), term2(t2) {}

  void initialize_call() { engine->unify(term1, term2); }

private:
  TermPtr term1;
  TermPtr term2;
};

// Loop body test pred
class BodyPred : public DetPred {
public:
  BodyPred(Engine *eng, TermPtr t1, TermPtr t2)
      : DetPred(eng), term1(t1), term2(t2) {}

  void initialize_call() { engine->unify(term1, term2); }

private:
  TermPtr term1;
  TermPtr term2;
};

//
// Test Loop
class TestBodyFactory : public LoopBodyFactory {
public:
  std::vector<PVarPtr> vars;
  std::vector<TermPtr> values;
  TestBodyFactory(Engine *eng, std::vector<PVarPtr> &vs,
                  std::vector<TermPtr> vals)
      : LoopBodyFactory(eng), vars(vs), values(vals) {
    index = 0;
  }

  bool loop_continues() override { return index < vars.size(); }

  PredPtr make_body_pred() override {
    TermPtr t1 = vars[index];
    TermPtr t2 = values[index++];
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
  static void test_trail(Engine *engine, PVarPtr var) {
    engine->trail(var);
    REQUIRE(engine->trail_stack.size() == 1);
    REQUIRE(engine->trail_stack.top()->var == var);
  }

  static void test_backtrack(Engine *engine, PVarPtr var) {
    engine->trail(var);
    var->bind(NEW_PINT(42));
    engine->backtrack();
    REQUIRE(engine->trail_stack.size() == 0);
    REQUIRE(var->value == nullptr);
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
  PVarPtr var = NEW_PVAR();
  PredPtr pred = std::make_shared<TestPred>(&engine);
  EngineTest::do_push(&engine, pred);

  SECTION("Trail test") { EngineTest::test_trail(&engine, var); }

  SECTION("Backtrack test") { EngineTest::test_backtrack(&engine, var); }

  SECTION("Push check") { EngineTest::test_push(&engine, pred); }
}

TEST_CASE("Engine execute test", "[Engine]") {
  Engine engine;
  PVarPtr var = NEW_PVAR();
  std::list<TermPtr> results;
  PredPtr failpred = std::make_shared<CollectAndFail>(&engine, var, results);
  PIntPtr term1 = NEW_PINT(42);
  PIntPtr term2 = NEW_PINT(43);
  std::vector<TermPtr> choices = {term1, term2};

  std::shared_ptr<VarChoiceIterator> choice_iterator1 =
      std::make_shared<VarChoiceIterator>(&engine, var, choices);
  std::shared_ptr<VarChoiceIterator> choice_iterator2 =
      std::make_shared<VarChoiceIterator>(&engine, var, choices);

  PredPtr choicePred1 = std::make_shared<ChoicePred>(&engine, choice_iterator1);
  PredPtr choicePred2 = std::make_shared<ChoicePred>(&engine, choice_iterator2);
  choicePred2->wrap_with_once();

  PredPtr conjunctionPred1 = conjunction({choicePred1, failpred});
  PredPtr conjunctionPred2 = conjunction({choicePred2, failpred});

  SECTION("Execute test - test once") {
    REQUIRE(choicePred2->get_continuation()->get_continuation() == failpred);
  }
  SECTION("Execute test - simple failure") {
    REQUIRE(!EngineTest::test_execute(&engine, failpred, false));
    REQUIRE(results.size() == 1);
    REQUIRE(results.front() == var);
  }

  SECTION("Execute test - conjunction and backtrack over choice") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred1, false));
    REQUIRE(results.size() == 2);
    REQUIRE(results.front() == term1);
    REQUIRE(results.back() == term2);
  }
  SECTION("Execute test - once, conjunction and backtrack over choice") {
    REQUIRE(var->value == nullptr);
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred2, false));
    REQUIRE(results.size() == 1);
    REQUIRE(results.front() == term1);
  }
}

TEST_CASE("Test backtracking over deterministic predicate", "[Engine]") {
  Engine engine;
  PVarPtr var = NEW_PVAR();
  PIntPtr term42 = NEW_PINT(42);

  PredPtr failpred = std::make_shared<Fail>(&engine);
  PredPtr detpred = std::make_shared<DetPredTest>(&engine, var, term42);
  PredPtr conjunctionPred = conjunction({detpred, failpred});

  SECTION("Test backtracking over deterministic predicate") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred, false));
    REQUIRE(var->value == nullptr);
  }
}

TEST_CASE("Engine execute disj test", "[Engine]") {
  Engine engine;
  PVarPtr var = NEW_PVAR();
  std::list<TermPtr> results;
  PredPtr failpred = std::make_shared<CollectAndFail>(&engine, var, results);
  PIntPtr term1 = NEW_PINT(42);
  PIntPtr term2 = NEW_PINT(43);
  std::vector<TermPtr> choices = {term1, term2};

  std::shared_ptr<VarChoiceIterator> choice_iterator1 =
      std::make_shared<VarChoiceIterator>(&engine, var, choices);
  std::shared_ptr<VarChoiceIterator> choice_iterator2 =
      std::make_shared<VarChoiceIterator>(&engine, var, choices);

  PredPtr choicePred1 = std::make_shared<ChoicePred>(&engine, choice_iterator1);
  PredPtr choicePred2 = std::make_shared<ChoicePred>(&engine, choice_iterator2);
  std::vector<PredPtr> disjPreds = {choicePred1, choicePred2};
  PredPtr disjPred = std::make_shared<DisjPred>(
      &engine, disjPreds); // Disjunction of choicePred1 and choicePred2

  PredPtr conjunctionPred3 = conjunction({disjPred, failpred});

  SECTION("Execute test - disjunction, backtrack over choice") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred3, false));
    REQUIRE(results.size() == 4);
    std::list<TermPtr>::iterator it = results.begin();
    REQUIRE(*it == term1);
    ++it;
    REQUIRE(*it == term2);
    ++it;
    REQUIRE(*it == term1);
    ++it;
    REQUIRE(*it == term2);
  }
}

TEST_CASE("Engine execute notnot predicate - not not call succeeds",
          "[Engine]") {
  Engine engine;
  PVarPtr var = NEW_PVAR();
  std::list<TermPtr> results;
  PredPtr failpred = std::make_shared<CollectAndFail>(&engine, var, results);
  PIntPtr term1 = NEW_PINT(42);
  PIntPtr term2 = NEW_PINT(43);
  std::vector<TermPtr> choices = {term1, term2};
  std::shared_ptr<VarChoiceIterator> choice_iterator1 =
      std::make_shared<VarChoiceIterator>(&engine, var, choices);
  PredPtr choicePred1 = std::make_shared<ChoicePred>(&engine, choice_iterator1);
  PredPtr notnot_pred = std::make_shared<NotNot>(&engine, choicePred1);
  PredPtr conjunctionPred = conjunction({notnot_pred, failpred});

  SECTION("Engine execute notnot predicate - execute") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred, false));
    REQUIRE(results.size() == 1);
    REQUIRE(results.front() == var);
  }
}

TEST_CASE("Engine execute notnot predicate - not not call fails", "[Engine]") {
  Engine engine;
  PVarPtr var = NEW_PVAR();
  std::list<TermPtr> results;
  PredPtr failpred = std::make_shared<CollectAndFail>(&engine, var, results);

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
  PVarPtr v1 = NEW_PVAR();
  PVarPtr v2 = NEW_PVAR();
  PVarPtr v3 = NEW_PVAR();
  PVarPtr v4 = NEW_PVAR();
  PIntPtr i1 = NEW_PINT(1);
  PIntPtr i2 = NEW_PINT(2);
  PIntPtr i3 = NEW_PINT(3);
  PIntPtr i4 = NEW_PINT(4);

  std::vector<PVarPtr> vars = {v1, v2, v3};
  std::vector<TermPtr> values = {i1, i2, i3};
  PredPtr detpred = std::make_shared<DetPredTest>(&engine, v4, i4);
  TestBodyFactory body_factory = TestBodyFactory(&engine, vars, values);
  PredPtr loop = std::make_shared<Loop>(&engine, &body_factory);
  PredPtr conjunctionPred = conjunction({loop, detpred});

  SECTION("Engine execute loop") {
    REQUIRE(EngineTest::test_execute(&engine, conjunctionPred, false));
    REQUIRE(v1->dereference() == i1);
    REQUIRE(v2->dereference() == i2);
    REQUIRE(v3->dereference() == i3);
    REQUIRE(v4->dereference() == i4);
  }
}

TEST_CASE("Test if-the-else predicate: if succeeds", "[Engine]") {
  Engine engine;
  PVarPtr v1 = NEW_PVAR();
  PVarPtr v2 = NEW_PVAR();
  PIntPtr i1 = NEW_PINT(1);
  PIntPtr i2 = NEW_PINT(2);
  PIntPtr i3 = NEW_PINT(3);
  PIntPtr i4 = NEW_PINT(4);
  PIntPtr i5 = NEW_PINT(5);
  std::list<TermPtr> results;
  std::vector<TermPtr> if_choices = {i1, i2};
  std::vector<TermPtr> then_choices = {i2, i3};
  std::vector<TermPtr> else_choices = {i4, i5};

  PredPtr failpred = std::make_shared<CollectAndFail>(&engine, v2, results);

  std::shared_ptr<VarChoiceIterator> choice_iterator_if =
      std::make_shared<VarChoiceIterator>(&engine, v1, if_choices);
  PredPtr ifPred = std::make_shared<ChoicePred>(&engine, choice_iterator_if);

  std::shared_ptr<VarChoiceIterator> choice_iterator_then =
      std::make_shared<VarChoiceIterator>(&engine, v2, then_choices);
  PredPtr thenPred =
      std::make_shared<ChoicePred>(&engine, choice_iterator_then);

  std::shared_ptr<VarChoiceIterator> choice_iterator_else =
      std::make_shared<VarChoiceIterator>(&engine, v2, else_choices);
  PredPtr elsePred =
      std::make_shared<ChoicePred>(&engine, choice_iterator_else);

  PredPtr if_then_else =
      std::make_shared<IfThenElse>(&engine, ifPred, thenPred, elsePred);
  PredPtr conjunctionPred = conjunction({if_then_else, failpred});

  SECTION("Engine execute") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred, false));
    REQUIRE(results.size() == 2);
    std::list<TermPtr>::iterator it = results.begin();
    REQUIRE(*it == i2);
    ++it;
    REQUIRE(*it == i3);
  }
}

TEST_CASE("Test if-the-else predicate: if fails", "[Engine]") {
  Engine engine;
  PVarPtr v1 = NEW_PVAR();
  PVarPtr v2 = NEW_PVAR();
  PIntPtr i1 = NEW_PINT(1);
  PIntPtr i2 = NEW_PINT(2);
  PIntPtr i3 = NEW_PINT(3);
  PIntPtr i4 = NEW_PINT(4);
  PIntPtr i5 = NEW_PINT(5);
  std::list<TermPtr> results;
  std::vector<TermPtr> then_choices = {i2, i3};
  std::vector<TermPtr> else_choices = {i4, i5};

  PredPtr failpred = std::make_shared<CollectAndFail>(&engine, v2, results);

  PredPtr ifPred = std::make_shared<Fail>(&engine);

  std::shared_ptr<VarChoiceIterator> choice_iterator_then =
      std::make_shared<VarChoiceIterator>(&engine, v2, then_choices);
  PredPtr thenPred =
      std::make_shared<ChoicePred>(&engine, choice_iterator_then);

  std::shared_ptr<VarChoiceIterator> choice_iterator_else =
      std::make_shared<VarChoiceIterator>(&engine, v2, else_choices);
  PredPtr elsePred =
      std::make_shared<ChoicePred>(&engine, choice_iterator_else);

  PredPtr if_then_else =
      std::make_shared<IfThenElse>(&engine, ifPred, thenPred, elsePred);
  PredPtr conjunctionPred = conjunction({if_then_else, failpred});

  SECTION("Engine execute") {
    REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred, false));
    REQUIRE(results.size() == 2);
    std::list<TermPtr>::iterator it = results.begin();
    REQUIRE(*it == i4);
    ++it;
    REQUIRE(*it == i5);
  }
}