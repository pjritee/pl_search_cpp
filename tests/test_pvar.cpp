#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "pl_search/pvar.hpp"
#include "pl_search/pint.hpp"

using namespace pl_search;

// Class for testing user defined variables

class UserVar : public PVar {

public:
    UserVar() : PVar() {}

    bool bind(Term* t) override;

    void set_disjoint(std::vector<Term*> disj) {
        disjoint = disj;
    }

    ~UserVar() {}

private:
 std::vector<Term*> disjoint;

};

bool UserVar::bind(Term* t) {
    for (auto it = disjoint.begin(); it != disjoint.end(); ++it) {
        if ( *t == *(*it)) return false;
    }
    return PVar::bind(t);

}

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

TEST_CASE("Test user defined variable", "[UserVar]") {
    UserVar v1, v2, v3;
    PInt n1(1);
    PInt n2(2);
    PInt n3(3);

    v1.set_disjoint({&v2, &v3});
    v2.set_disjoint({&v1, &v3});
    v3.set_disjoint({&v1, &v2});

    SECTION("Attempt to bind to disjoint var") {
        REQUIRE(!v1.bind(&v2));

    }
    SECTION("Attempt to bind to number") {
        REQUIRE(v1.bind(&n1));
        REQUIRE(!v2.bind(&n1));

    }
}

TEST_CASE("TEST is_var", "[PVar]") {
    PVar v1, v2;
    PInt n(42);

    SECTION("Test is_var on unbound terms") {
        REQUIRE(v1.is_var());
        REQUIRE(!n.is_var());
    }

    SECTION("Test is_var for var to var binding") {
        REQUIRE(v1.bind(&v2));
        REQUIRE(v1.is_var());
    }
    SECTION("Test is_var for var to int binding") {
        REQUIRE(v1.bind(&n));
        REQUIRE(!v1.is_var());
    }
}