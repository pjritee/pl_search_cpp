#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "pl_search/engine.hpp"
#include "pl_search/pred.hpp"
#include "pl_search/pvar.hpp"
#include "pl_search/pint.hpp"
#include "pl_search/choice_iterator.hpp"


using namespace pl_search;

TEST_CASE("ChoicePred functionality", "[ChoicePred]") {
    Engine engine;
    PInt term1(42);
    PInt term2(43);
    PVar var;
    std::vector<Term*> choices = {&term1, &term2};
    VarChoiceIterator choice_iterator(&engine, &var, choices);


    std::shared_ptr<ChoicePred> choicePred = std::make_shared<ChoicePred>(&engine, &choice_iterator);
    
    engine.push(choicePred); // added so we can backtrack


    SECTION("Apply choice test") {
        // initially stack is [42, 43]
        REQUIRE(choicePred->apply_choice());
        // stack is now [42]
        REQUIRE(var.dereference() == &term2); // var is now bound to 43
 
        engine.backtrack(); // backtrack to before the choice was applied
        REQUIRE(choicePred->more_choices()); // stack is now [42]
        REQUIRE(choicePred->apply_choice());
        // stack is now empty
        REQUIRE(var.dereference() == &term1); // var is now bound to 42
        engine.backtrack(); // backtrack to before the choice was applied
        REQUIRE(!choicePred->more_choices()); // stack is now empty
        
    }
}
