#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "pl_search/pred.hpp"
#include "pl_search/engine.hpp"
#include "pl_search/term.hpp"
#include "pl_search/pvar.hpp"
#include "pl_search/pint.hpp"
#include "pl_search/choice_iterator.hpp"


using namespace pl_search;

// Like Prolog unify, but only for testing
class SemiDetTestPred : public SemiDetPred {
 public:
  SemiDetTestPred(Engine* eng, Term* t1, Term* t2) : SemiDetPred(eng), term1(t1), term2(t2) {}
  void initialize_call() override {
    // Add assertions based on what initialize_call is supposed to do
  }
  bool test_call() {
    return engine->unify(term1, term2);
  }
  private:
  Term* term1;
  Term* term2;
};

// line the true Prolog predicate
class DetTestPred : public DetPred {
 public:
  DetTestPred(Engine* eng) : DetPred(eng){}
  void initialize_call() override {
    
  } 
};


TEST_CASE("ChoicePred functionality", "[ChoicePred]") {
    Engine engine;
    PInt term1(42);
    PInt term2(43);
    PVar var;
    std::vector<Term*> choices = {&term1, &term2};
    VarChoiceIterator choice_iterator(&engine, &var, choices);

    std::shared_ptr<ChoicePred> choicePred = 
        std::make_shared<ChoicePred>(&engine, &choice_iterator);
 
    
    SECTION("Apply choice test") {
        REQUIRE(choicePred->apply_choice() == true);
    }

    SECTION("Test choice test") {
        REQUIRE(choicePred->test_choice() == true);
    }

    SECTION("More choices test") {
        REQUIRE(choicePred->more_choices() == true);
    }
}

TEST_CASE("SemiDetPred functionality", "[SemiDetPred]") {
    Engine engine;
    PInt term1(42);
    PInt term2(43);
    std::shared_ptr<SemiDetTestPred> semiDetPred12 = std::make_shared<SemiDetTestPred>(&engine, &term1, &term2);
    std::shared_ptr<SemiDetTestPred> semiDetPred11 = std::make_shared<SemiDetTestPred>(&engine, &term1, &term1);
    
    SECTION("Try call test term1  and term2") {
        REQUIRE(semiDetPred12->test_call() == false);
    }

    SECTION("Try call test term1  and term1") {
        REQUIRE(semiDetPred11->test_call() == true);
    }
}

TEST_CASE("Conjunction functionality", "[Conjunction]") {
    Engine engine;
    PInt term1(42);
    PInt term2(43);
    std::shared_ptr<SemiDetTestPred> semiDetPred12 = std::make_shared<SemiDetTestPred>(&engine, &term1, &term2);
    std::shared_ptr<SemiDetTestPred> semiDetPred11 = std::make_shared<SemiDetTestPred>(&engine, &term1, &term1);
    std::vector<PredPtr> preds = {semiDetPred12, semiDetPred11};
    
    PredPtr conjunctionPred = conjunction(preds);

    
    SECTION("Try call test") {
        REQUIRE(conjunctionPred == semiDetPred12);
        REQUIRE(conjunctionPred->get_continuation() == semiDetPred11);
    }
}



