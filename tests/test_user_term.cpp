#define CATCH_CONFIG_MAIN
#include "pl_search/engine.hpp"
#include "pl_search/patom.hpp"
#include "pl_search/pvar.hpp"
#include <catch2/catch.hpp>

using namespace pl_search;

static PAtom empty_list = PAtom("[]");

class PrologList : public Term {

public:
  Term *head;
  Term *tail;

  PrologList(Term *h, Term *t) : Term(), head(h), tail(t) {}

  Term *dereference() override { return this; }

  bool bind(Term *t) override { return false; }

  void reset(Term *t) override {}

  bool isEqualTo(Term &t) override {
    std::cerr << *this << " other " << t << std::endl;
    PrologList *lst = dynamic_cast<PrologList *>(&t);
    if (lst == nullptr) {
      return false;
    }
    return (head->isEqualTo(*(lst->head)->dereference())) &&
           tail->isEqualTo(*(lst->tail->dereference()));
  }

  bool isLessThan(Term &t) override {
    PrologList *lst = dynamic_cast<PrologList *>(&t);
    // not less than any 'system' type
    if (lst == nullptr) {
      return false;
    }
    if (head->isLessThan(*(lst->head->dereference()))) {
      return true;
    }
    return tail->isLessThan(*(lst->tail->dereference()));
  }

  bool unifyWith(Engine *engine, Term *t) override {
    // as unification with a variable is already handled so this will only unify
    // with t if t is a PrologList
    std::cerr << "unify " << *this << " other " << t << std::endl;
    PrologList *lst = dynamic_cast<PrologList *>(t);
    if (lst == nullptr) {
      return engine->unify(this, t);
    }
    return (engine->unify(head, lst->head)) && (engine->unify(tail, lst->tail));
  }

  std::string repr() const {
    std::ostringstream oss;
    oss << "[";
    oss << *head;
    Term *list_tail = tail->dereference();
    PrologList *tail_lst = dynamic_cast<PrologList *>(list_tail);
    while (tail_lst != nullptr) {
      list_tail = tail_lst->tail->dereference();
      oss << ", " << *(tail_lst->head->dereference());
      tail_lst = dynamic_cast<PrologList *>(list_tail);
    }
    if (*list_tail == empty_list) {
      oss << "]";
      return oss.str();
    }
    oss << "|" << *list_tail << "]";
    return oss.str();
  }
};

TEST_CASE("User defined term type - build and test", "[Term]") {
  Engine engine;
  PVar v1, v2, v3;
  PrologList list1 = PrologList(&v3, &empty_list);
  PrologList list2 = PrologList(&v2, &list1);
  PrologList list3 = PrologList(&v2, &v1);

  SECTION("Test displaying list") {
    REQUIRE(list2.head == &v2);
    REQUIRE(list2.tail == &list1);
    REQUIRE(list1.head == &v3);
    REQUIRE(list1.tail == &empty_list);
    REQUIRE(list2.repr() == "[X1, X2]");
  }
  SECTION("Test unify") {
    REQUIRE(engine.unify(&list2, &list3));
    REQUIRE(v1.dereference()->isEqualTo(list1));
  }
}