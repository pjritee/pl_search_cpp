#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "pl_search/engine.hpp"
#include "pl_search/pvar.hpp"
#include "pl_search/pint.hpp"

using namespace pl_search;

TEST_CASE("Engine trail, backtrack and unify", "[Engine]") {
    Engine engine;
    PVar var;
    PVar v;
    PInt term(42);
    PInt term2(43);
    PInt term3(42);

    SECTION("Trail test") {
        engine.trail(&var);
        REQUIRE(engine.trail_stack.size() == 1);
        REQUIRE(engine.trail_stack.top()->var == &var);
        REQUIRE(engine.trail_stack.top()->value == &var);
    }

    SECTION("Backtrack test") {
        // Push a nullptr onto the trail stack to initialize env_stack 
        // with initial top of trail_stack index (= 0)
        engine.push(nullptr);
        engine.trail(&var);
        var.bind(&term);
        engine.backtrack();  
        REQUIRE(engine.trail_stack.size() == 0);
        REQUIRE(var.value == &var);
    }

    SECTION("Unify test") {
        // Unify two PInt objects with different values
        REQUIRE(engine.unify(&term, &term2) == false);
        // Unify two PInt objects with the same value
        REQUIRE(engine.unify(&term, &term3) == true);  
        // Unify two PVar objects
        REQUIRE(engine.unify(&v, &var) == true);
        // v should be bound to var and so should dereference to var
        REQUIRE(v.dereference() == &var);
        // Unify term and v and because v dereferences to var, var should be bound to term
        REQUIRE(engine.unify(&term, &v) == true);
        REQUIRE(var.dereference() == &term);
    }
}

//TEST_CASE("Engine execute", "[Engine]") {
//    Engine engine;
//    // Add more setup and assertions as needed for your specific use case
//    REQUIRE(engine.execute() == false);
//}