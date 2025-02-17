#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "pl_search/engine.hpp"
#include "pl_search/pvar.hpp"
#include "pl_search/pint.hpp"
#include "pl_search/pred.hpp"
#include "pl_search/choice_iterator.hpp"

using namespace pl_search;

// Define a simple predicate for testing
class TestPred : public Pred {
public:
    TestPred(Engine* eng) : Pred(eng) {}

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
    std::list<Term*>& results;
    PVar* var;
    CollectAndFail(Engine* eng, PVar* v, std::list<Term*>& r) : SemiDetPred(eng),
        var(v), results(r) {}

    void initialize_call() override {
       results.push_back(var->dereference());
    }

    bool apply_choice() override {
        return false;
    }

    bool test_choice() override {
        return false;
    }
};

// Equivalent of Prolog fail
class Fail : public SemiDetPred {
    public:
        Fail(Engine* eng) : SemiDetPred(eng){}
    
        void initialize_call() override {
        }
    
        bool apply_choice() override {
            return false;
        }
    
        bool test_choice() override {
            return false;
        }
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
    static void do_push(Engine* engine, PredPtr pred) {
        engine->push(pred);
    }
    static void test_trail(Engine* engine, PVar* var) {
        engine->trail(var);
        REQUIRE(engine->trail_stack.size() == 1);
        REQUIRE(engine->trail_stack.top()->var == var);
    }

    static void test_backtrack(Engine* engine, PVar* var) {
        engine->trail(var);
        var->bind(new PInt(42));
        engine->backtrack();
        REQUIRE(engine->trail_stack.size() == 0);
        REQUIRE(var->value == var);
    }

    static void test_push(Engine* engine, PredPtr pred) {
        REQUIRE(engine->env_stack.size() == 1);
        REQUIRE(engine->env_stack.top()->pred == pred);
    }


    static bool test_execute(Engine* engine, PredPtr pred, bool unbind) {
        return engine->execute(pred, unbind);
    }
};

TEST_CASE("Engine private member tests", "[Engine]") {
    Engine engine;
    PVar var;
    PredPtr pred = std::make_shared<TestPred>(&engine);
    EngineTest::do_push(&engine, pred);

    SECTION("Trail test") {
        EngineTest::test_trail(&engine, &var);
    }

    SECTION("Backtrack test") {
        EngineTest::test_backtrack(&engine, &var);
    }

    SECTION("Push check") {
        EngineTest::test_push(&engine, pred);
    }

}

TEST_CASE("Engine execute test", "[Engine]") {
    Engine engine;
    PVar var;
    std::list<Term*> results;
    PredPtr failpred = std::make_shared<CollectAndFail>(&engine, &var, results);
    PInt term1(42);
    PInt term2(43);
    std::vector<Term*> choices = {&term1, &term2};
    VarChoiceIterator choice_iterator1(&engine, &var, choices);
    VarChoiceIterator choice_iterator2(&engine, &var, choices);
    PredPtr choicePred1 = std::make_shared<ChoicePred>(&engine, &choice_iterator1);
    PredPtr choicePred2 = std::make_shared<ChoicePred>(&engine, &choice_iterator2);
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

TEST_CASE("Engine execute disj test", "[Engine]") {
    Engine engine;
    PVar var;
    std::list<Term*> results;
    PredPtr failpred = std::make_shared<CollectAndFail>(&engine, &var, results);
    PInt term1(42);
    PInt term2(43);
    std::vector<Term*> choices = {&term1, &term2};
    VarChoiceIterator choice_iterator1(&engine, &var, choices);
    VarChoiceIterator choice_iterator2(&engine, &var, choices);
    PredPtr choicePred1 = std::make_shared<ChoicePred>(&engine, &choice_iterator1);
    PredPtr choicePred2 = std::make_shared<ChoicePred>(&engine, &choice_iterator2);
 
    std::cout << "failpred " << repr(failpred) << std::endl;
    std::cout << "choicepred1 " << repr(choicePred1) << std::endl;
    std::cout << "choicepred2 " << repr(choicePred2) << std::endl;
 
    PredPtr disjPred = std::make_shared<DisjPred>(&engine, std::vector<PredPtr>{choicePred1, choicePred2});
    std::cout << "disjPred " << repr(disjPred) << std::endl;
    PredPtr conjunctionPred3 = conjunction({disjPred, failpred});
    std::cout << "conjPred3 " << repr(conjunctionPred3) << std::endl;

 
    SECTION("Execute test - disjunction, backtrack over choice") {
        REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred3, false));
        REQUIRE(results.size() == 4);
        std::list<Term*>::iterator it = results.begin();
        REQUIRE(*it == &term1);
        ++it;
        REQUIRE(*it == &term2);
        ++it;
        REQUIRE(*it == &term1);
        ++it;
        REQUIRE(*it == &term2);
    }
}

TEST_CASE("Engine execute notnot predicate - not not call succeeds", "[Engine]") {
    Engine engine;
    PVar var;
    std::list<Term*> results;
    PredPtr failpred = std::make_shared<CollectAndFail>(&engine, &var, results);
    PInt term1(42);
    PInt term2(43);
    std::vector<Term*> choices = {&term1, &term2};
    VarChoiceIterator choice_iterator1(&engine, &var, choices);
    PredPtr choicePred1 = std::make_shared<ChoicePred>(&engine, &choice_iterator1);
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
    std::list<Term*> results;
    PredPtr failpred = std::make_shared<CollectAndFail>(&engine, &var, results);
    PInt term1(42);
    PInt term2(43);
    std::vector<Term*> choices = {&term1, &term2};
    VarChoiceIterator choice_iterator1(&engine, &var, choices);
    PredPtr failPred= std::make_shared<Fail>(&engine);
    PredPtr notnotPred = std::make_shared<NotNot>(&engine, failPred);
    PredPtr conjunctionPred = conjunction({notnotPred, failpred});

    SECTION("Engine execute notnot predicate - execute") {
        REQUIRE(!EngineTest::test_execute(&engine, conjunctionPred, false));
        REQUIRE(results.size() == 0);
    }
}