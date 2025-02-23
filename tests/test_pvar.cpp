#define CATCH_CONFIG_MAIN
#include "pl_search/pint.hpp"
#include "pl_search/pvar.hpp"
#include <catch2/catch.hpp>

using namespace pl_search;

// Class for testing user defined variables

class UserVar : public PVar {

public:
  UserVar() : PVar() {}

  bool bind(TermPtr t) override;

  void set_disjoint(std::vector<TermPtr> disj) { disjoint = disj; }

  ~UserVar() {}

private:
  std::vector<TermPtr> disjoint;
};

bool UserVar::bind(TermPtr t) {
  for (auto it = disjoint.begin(); it != disjoint.end(); ++it) {
    if (*t == *(*it))
      return false;
  }
  return PVar::bind(t);
}

TEST_CASE("PVar binding", "[PVar]") {
  PVarPtr pvar = std::make_shared<PVar>();
  PIntPtr term =
      std::make_shared<PInt>(42); // Example term with integer value 42

  SECTION("Bind test") {
    REQUIRE(pvar->bind(term));
    REQUIRE(pvar->value == term);
  }

  // SECTION("Reset test") {
  //   pvar->reset(nullptr);
  //   REQUIRE(pvar->value == nullptr);
  // }

  // SECTION("IsLessThan test") {
  //   PVarPtr anotherPVar = std::make_shared<PVar>();
  //   REQUIRE(pvar->isLessThan(*anotherPVar));
  // }
}

// TEST_CASE("PVar dereference", "[PVar]") {
//   PVarPtr pvar = std::make_shared<PVar>();
//   PIntPtr term =
//       std::make_shared<PInt>(42); // Example term with integer value 42

//   SECTION("Dereference unbound variable") {
//     REQUIRE(*(pvar->dereference()) == *pvar);
//   }

//   SECTION("Dereference bound variable") {
//     pvar->bind(term);
//     REQUIRE(*(pvar->dereference()) == *term);
//   }
// }

// TEST_CASE("UpdatablePVar functionality", "[UpdatablePVar]") {

//   PVarPtr v = std::make_shared<PVar>();
//   PIntPtr term =
//       std::make_shared<PInt>(42); // Example term with integer value 42
//   UpdatablePVarPtr upvar = std::make_shared<UpdatablePVar>(
//       v); // Updatable variable initialized with v

//   SECTION("Dereference test unbound") {
//     REQUIRE(*(upvar->dereference()) == *upvar);
//     REQUIRE(*(upvar->value->dereference()) == *v);
//   }
//   SECTION("Dereference test bound") {
//     v->bind(term);
//     REQUIRE(*(upvar->dereference()) == *upvar);
//     REQUIRE(*(upvar->value->dereference()) == *term);
//   }
// }

// TEST_CASE("Test user defined variable", "[UserVar]") {
//   std::shared_ptr<UserVar> v1 = std::make_shared<UserVar>();
//   std::shared_ptr<UserVar> v2 = std::make_shared<UserVar>();
//   std::shared_ptr<UserVar> v3 = std::make_shared<UserVar>();
//   PIntPtr n1 = std::make_shared<PInt>(1);
//   PIntPtr n2 = std::make_shared<PInt>(2);
//   PIntPtr n3 = std::make_shared<PInt>(3);

//   v1->set_disjoint({v2, v3});
//   v2->set_disjoint({v1, v3});
//   v3->set_disjoint({v1, v2});

//   SECTION("Attempt to bind to disjoint var") { REQUIRE(!v1->bind(v2)); }
//   SECTION("Attempt to bind to number") {
//     REQUIRE(v1->bind(n1));
//     REQUIRE(!v2->bind(n1));
//   }
// }

// TEST_CASE("TEST is_var", "[PVar]") {
//   PVarPtr v1 = std::make_shared<PVar>();
//   PVarPtr v2 = std::make_shared<PVar>();
//   PIntPtr n1 = std::make_shared<PInt>(42);

//   SECTION("Test is_var on unbound terms") {
//     REQUIRE(v1->is_var());
//     REQUIRE(!n1->is_var());
//   }

//   SECTION("Test is_var for var to var binding") {
//     REQUIRE(v1->bind(v2));
//     REQUIRE(v1->is_var());
//   }
//   SECTION("Test is_var for var to int binding") {
//     REQUIRE(v1->bind(n1));
//     REQUIRE(!v1->is_var());
//   }
// }