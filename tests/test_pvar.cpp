#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "pl_search/pvar.hpp"
#include "pl_search/pint.hpp"

using namespace pl_search;

TEST_CASE("PVar binding", "[PVar]") {
    PVar pvar;
    PInt term(42); // Example term with integer value 42

    SECTION("Bind test") {
        REQUIRE(pvar.bind(&term));
        REQUIRE(pvar.value == &term);
    }

    SECTION("Reset test") {
        pvar.reset(&pvar);
        REQUIRE(pvar.value == &pvar);
    }

    SECTION("IsLessThan test") {
        PVar anotherPVar;
        REQUIRE(pvar.isLessThan(anotherPVar));
    }
}

TEST_CASE("PVar dereference", "[PVar]") {
    PVar pvar;
    PInt term(42); // Example term with integer value 42

    SECTION("Dereference unbound variable") {
        REQUIRE(pvar.dereference() == &pvar);
    }

    SECTION("Dereference bound variable") {
        pvar.bind(&term);
        REQUIRE(pvar.dereference() == &term);
    }
}

TEST_CASE("UpdatablePVar functionality", "[UpdatablePVar]") {
  
    PVar v; // Example variable
    PInt term(42); // Example term with integer value 42
    UpdatablePVar upvar(&v); // Updatable variable initialized with v

    
    SECTION("Dereference test unbound") {
        REQUIRE(upvar.dereference() == &upvar);
        REQUIRE(upvar.value->dereference() == &v);
    }
   SECTION("Dereference test bound") {
        v.bind(&term);
        REQUIRE(upvar.dereference() == &upvar);
        REQUIRE(upvar.value->dereference() == &term);
    }
}