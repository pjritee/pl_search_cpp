#define CATCH_CONFIG_MAIN
#include "pl_search/choice_iterator.hpp"
#include "pl_search/engine.hpp"
#include "pl_search/pint.hpp"
#include "pl_search/pred.hpp"
#include "pl_search/pvar.hpp"
#include "pl_search/term.hpp"
#include <catch2/catch.hpp>

using namespace pl_search;

// Like Prolog unify, but only for testing
class SemiDetTestPred : public SemiDetPred {
public:
  SemiDetTestPred(Engine *eng, TermPtr t1, TermPtr t2)
      : SemiDetPred(eng), term1(t1), term2(t2) {}
  void initialize_call() override {
    // Add assertions based on what initialize_call is supposed to do
  }
  bool test_call() { return engine->unify(term1, term2); }

private:
  TermPtr term1;
  TermPtr term2;
};

// line the true Prolog predicate
class DetTestPred : public DetPred {
public:
  DetTestPred(Engine *eng) : DetPred(eng) {}
  void initialize_call() override {}
};

//
// Test Loop
class TestBodyFactory : public LoopBodyFactory {
public:
  TestBodyFactory(Engine *eng) : LoopBodyFactory(eng) { cont = true; }

  bool loop_continues() override {
    if (cont) {
      cont = false;
      return true;
    }
    return false;
  }

  PredPtr make_body_pred() override {
    return std::make_shared<DetTestPred>(engine);
  }

private:
  bool cont;
};

TEST_CASE("ChoicePred functionality", "[ChoicePred]") {
  Engine engine;
  PIntPtr term1 = NEW_PINT(42);
  PIntPtr term2 = NEW_PINT(43);
  std::vector<TermPtr> choices = {term1, term2};
  PVarPtr var = NEW_PVAR();

  std::shared_ptr<VarChoiceIterator> choice_iterator =
      std::make_shared<VarChoiceIterator>(&engine, var, choices);

  std::shared_ptr<ChoicePred> choicePred =
      std::make_shared<ChoicePred>(&engine, choice_iterator);

  SECTION("Apply choice test") { REQUIRE(choicePred->apply_choice() == true); }

  SECTION("More choices test") { REQUIRE(choicePred->more_choices() == true); }
}

TEST_CASE("SemiDetPred functionality", "[SemiDetPred]") {
  Engine engine;
  PIntPtr term1 = NEW_PINT(42);
  PIntPtr term2 = NEW_PINT(43);
  std::shared_ptr<SemiDetTestPred> semiDetPred12 =
      std::make_shared<SemiDetTestPred>(&engine, term1, term2);
      
  std::shared_ptr<SemiDetTestPred> semiDetPred11 =
      std::make_shared<SemiDetTestPred>(&engine, term1, term1);

  SECTION("Try call test term1  and term2") {
    REQUIRE(semiDetPred12->test_call() == false);
  }

  SECTION("Try call test term1  and term1") {
    REQUIRE(semiDetPred11->test_call() == true);
  }
}

TEST_CASE("Conjunction functionality", "[Conjunction]") {
  Engine engine;
  PIntPtr term1 = NEW_PINT(42);
  PIntPtr term2 = NEW_PINT(43);
  std::shared_ptr<SemiDetTestPred> semiDetPred12 =
      std::make_shared<SemiDetTestPred>(&engine, term1, term2);
  std::shared_ptr<SemiDetTestPred> semiDetPred11 =
      std::make_shared<SemiDetTestPred>(&engine, term1, term1);

  std::vector<PredPtr> preds = {semiDetPred12, semiDetPred11};

  PredPtr conjunctionPred = conjunction(preds);

  SECTION("Try call test") {
    REQUIRE(conjunctionPred == semiDetPred12);
    REQUIRE(conjunctionPred->get_continuation() == semiDetPred11);
  }
}

TEST_CASE("Loop functionality", "[Loop]") {
  Engine engine;
  TestBodyFactory body_factory = TestBodyFactory(&engine);
  PredPtr loop = std::make_shared<Loop>(&engine, &body_factory);

  loop->initialize_call();

  SECTION("Test loop continuation") {
    REQUIRE(loop->apply_choice());
    REQUIRE(loop->get_continuation()->get_continuation() == loop);
    // loop_continue becomes false so the loop exits
    REQUIRE(loop->apply_choice());
    REQUIRE(loop->get_continuation() == nullptr);
  }
}
