/*
MIT License

Copyright (c) 2025 [Peter Robinson]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// An example of defining a user term - a Prolog list term

#include "pl_search/engine.hpp"
#include "pl_search/patom.hpp"
#include "pl_search/term.hpp"

#include <cassert>

#ifndef PROLOG_LIST_HPP
#define PROLOG_LIST_HPP

using namespace pl_search;

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

// A helper function to build a PrologList from the vector of elements
PrologListPtr make_list_from(std::vector<TermPtr> elements) {
  assert(elements.size() > 0);

  auto it = elements.rbegin();
  PrologListPtr lst = NEW_PROLOG_LIST(*it, empty_list);
  it++;
  for (; it != elements.rend(); it++) {
    lst = NEW_PROLOG_LIST(*it, lst);
  }
  return lst;
}

// Similar as above - when the (ultimate) tail is a vaiable this is usually
// called an open list.
PrologListPtr make_open_list_from(std::vector<TermPtr> elements) {
  assert(elements.size() > 1);
  assert(elements[elements.size() - 1]->is_var());

  auto it = elements.rbegin();
  auto e1 = *it;
  it++;
  PrologListPtr lst = NEW_PROLOG_LIST(*it, e1);
  it++;
  for (; it != elements.rend(); it++) {
    lst = NEW_PROLOG_LIST(*it, lst);
  }
  return lst;
}

#endif // PROLOG_LIST_HPP