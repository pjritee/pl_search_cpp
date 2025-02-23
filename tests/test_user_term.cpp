#define CATCH_CONFIG_MAIN
#include "pl_search/engine.hpp"
#include "pl_search/patom.hpp"
#include "pl_search/pvar.hpp"
#include <catch2/catch.hpp>

using namespace pl_search;

static PAtomPtr empty_list = std::make_shared<PAtom>("[]");

class PrologList : public Term {

public:
  TermPtr head;
  TermPtr tail;

  PrologList(TermPtr h, TermPtr t) : Term(), head(h), tail(t) {}

  TermPtr dereference() override { return shared_from_this(); }

  bool bind(TermPtr t) override { return false; }

  void reset(TermPtr) override {}

  bool isEqualTo(Term &t) const override {
    if (typeid(*this) != typeid(t)) {
      return false;
    }
    PrologList lst = static_cast<PrologList &>(t);
    return (head == lst.head) && (tail == lst.tail);
  }

  bool isLessThan(Term &t) const override {
    if (typeid(*this) != typeid(t)) {
      return false;
    }
    PrologList lst = static_cast<PrologList &>(t);
    if (head->isLessThan(*(lst.head->dereference()))) {
      return true;
    }
    return tail->isLessThan(*(lst.tail->dereference()));
  }

  bool unifyWith(Engine *engine, TermPtr t) override {
    // as unification with a variable is already handled so this will only
    // unify with t if t is a PrologList
    std::cerr << "unify " << *this << " other " << t << std::endl;
    if (typeid(*this) != typeid(*t)) {
      return false;
    }
    PrologList lst = static_cast<PrologList &>(*t);
    return engine->unify(head, lst.head) && engine->unify(tail, lst.tail);
  }

  std::string repr() const {
    std::ostringstream oss;
    oss << "[";
    oss << *(head->dereference());
    TermPtr list_tail = tail->dereference();
    while (typeid(*list_tail) == typeid(PrologList)) {
      PrologList tail_lst = static_cast<PrologList &>(*list_tail);
      list_tail = tail_lst.tail->dereference();
      oss << ", " << *(tail_lst.head->dereference());
    }
    if (list_tail == empty_list) {
      oss << "]";
      return oss.str();
    }
    oss << "|" << *list_tail << "]";
    return oss.str();
  }
};

typedef std::shared_ptr<PrologList> PrologListPtr;

TEST_CASE("User defined term type - build and test", "[Term]") {
  Engine engine;
  PVarPtr v1 = std::make_shared<PVar>();
  PVarPtr v2 = std::make_shared<PVar>();
  PVarPtr v3 = std::make_shared<PVar>();
  PrologListPtr list1 = std::make_shared<PrologList>(v3, empty_list);
  PrologListPtr list2 = std::make_shared<PrologList>(v2, list1);
  PrologListPtr list3 = std::make_shared<PrologList>(v2, v1);

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