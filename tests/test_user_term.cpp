#define CATCH_CONFIG_MAIN
#include "pl_search/engine.hpp"
#include "pl_search/patom.hpp"
#include "pl_search/pvar.hpp"
#include <catch2/catch.hpp>

using namespace pl_search;

// Copy of PrologList definition from examples/prolog_list.hpp

// Defining  the empty list
static PAtomPtr empty_list = std::make_shared<PAtom>("[]");

// Making PrologList consistent with the typedefs and macros in typedefs.hpp
class PrologList;
typedef std::shared_ptr<PrologList> PrologListPtr;
#define NEW_PROLOG_LIST std::make_shared<PrologList>

class PrologList : public Term {

public:
  TermPtr head;
  TermPtr tail;

  // A prolog list is the "cons" of the head of the list  and the
  // tail of the list (in Prolog [] is also considered a list).
  // The Prolog list is a recursive data type.
  PrologList(TermPtr h, TermPtr t) : Term(), head(h), tail(t) {}

  TermPtr dereference() override { return shared_from_this(); }

  bool bind(TermPtr t) override { return false; }

  void reset(TermPtr) override {}

  bool isEqualTo(Term &t) const override {
    PrologList *lst = dynamic_cast<PrologList *>(&t);
    if (lst == nullptr) {
      return false;
    }
    return (head == lst->head) && (tail == lst->tail);
  }

  bool isLessThan(Term &t) const override {
    PrologList *lst = dynamic_cast<PrologList *>(&t);
    if (lst == nullptr) {
      return false;
    }
    if (head->isLessThan(*(lst->head->dereference()))) {
      return true;
    }
    return tail->isLessThan(*(lst->tail->dereference()));
  }

  bool unifyWith(Engine *engine, TermPtr t) override {
    // as unification with a variable is already handled so this will only
    // unify with t if t is a PrologList
    // A PrologList only unifies with a variable (already handled by the
    // definition of PVar::unifyWith) or with another list and then only
    // when the heads unify and the tails unify
    if (PrologListPtr lst = std::dynamic_pointer_cast<PrologList>(t)) {
      return engine->unify(head, lst->head) && engine->unify(tail, lst->tail);
    }
    return false;
  }

  std::string repr() const {
    std::ostringstream oss;
    oss << "[";
    oss << *(head->dereference());
    TermPtr list_tail = tail->dereference();
    while (PrologListPtr tail_lst =
               std::dynamic_pointer_cast<PrologList>(list_tail)) {
      list_tail = tail_lst->tail->dereference();
      oss << ", " << *(tail_lst->head->dereference());
    }
    if (*list_tail == *empty_list) {
      oss << "]";
      return oss.str();
    }
    oss << "|" << *list_tail << "]";
    return oss.str();
  }
};

TEST_CASE("User defined term type - build and test", "[Term]") {
  Engine engine;
  PVarPtr v1 = NEW_PVAR();
  PVarPtr v2 = NEW_PVAR();
  PVarPtr v3 = NEW_PVAR();
  PrologListPtr list1 = NEW_PROLOG_LIST(v3, empty_list);
  PrologListPtr list2 = NEW_PROLOG_LIST(v2, list1);
  PrologListPtr list3 = NEW_PROLOG_LIST(v2, v1);

  SECTION("Test displaying list") {
    REQUIRE(list2->head == v2);
    REQUIRE(list2->tail == list1);
    REQUIRE(list1->head == v3);
    REQUIRE(list1->tail == empty_list);
    REQUIRE(list2->repr() == "[X1, X2]");
  }
  // SECTION("Test unify") {
  //   REQUIRE(engine.unify(&list2, &list3));
  //   REQUIRE(v1.dereference()->isEqualTo(list1));
  // }
}