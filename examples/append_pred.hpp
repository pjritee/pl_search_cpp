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

// An example of writing a Prolog-like predicate. Although this
// project isn't about making a version of Prolog some might
// find this interesting.

#include "pl_search/engine.hpp"
#include "pl_search/pred.hpp"
#include "pl_search/term.hpp"
#include "prolog_list.hpp"

using namespace pl_search;

// An approximation of the append predicate in Prolog
// Typically append is written in Prolog as
//
//  append([], L, L).
//  append([H|T], L2, [H|L3]) :- append(T, L2, L3).
//
// It could also be written as
//
// append(L1,L2,L3) :-
//     (
//        L1 = [], L2 = L3        % AppendClause1
//     ;
//       L1 = [H|T], L3 = [H|L4], append(T, L2, L4) % AppendClause2
//     ).

// The first choice above

class AppendClause1 : public SemiDetPred {

public:
  AppendClause1(Engine *eng, TermPtr arg1, TermPtr arg2, TermPtr arg3)
      : SemiDetPred(eng), l1(arg1), l2(arg2), l3(arg3) {}

  void initialize_call() override {}

  bool apply_choice() override {
    return engine->unify(l1, empty_list) && engine->unify(l2, l3);
  }

  bool test_choice() override { return true; }

private:
  TermPtr l1;
  TermPtr l2;
  TermPtr l3;
};

// The second choice above

class AppendClause2 : public SemiDetPred {

public:
  AppendClause2(Engine *eng, TermPtr arg1, TermPtr arg2, TermPtr arg3)
      : SemiDetPred(eng), l1(arg1), l2(arg2), l3(arg3) {}

  void initialize_call() override { saved_continuation = continuation; }

  bool apply_choice() override;

  bool test_choice() override { return true; }

private:
  PredPtr saved_continuation;
  TermPtr l1;
  TermPtr l2;
  TermPtr l3;
};

class Append : public Pred {
public:
  Append(Engine *eng, TermPtr arg1, TermPtr arg2, TermPtr arg3)
      : Pred(eng), l1(arg1), l2(arg2), l3(arg3), index(0) {}

  void initialize_call() override {
    preds = {std::make_shared<AppendClause1>(engine, l1, l2, l3),
             std::make_shared<AppendClause2>(engine, l1, l2, l3)};
    preds[0]->set_continuation(continuation);
    preds[1]->set_continuation(continuation);
  }

  bool more_choices() override { return index < preds.size(); }

  bool apply_choice() {
    PredPtr clause = preds[index++];
    continuation = clause;
    return true;
  }

  bool test_choice() override { return true; }

private:
  TermPtr l1;
  TermPtr l2;
  TermPtr l3;
  std::vector<PredPtr> preds;
  int index;
};

// Although this is a straightforward implementation it's somewhat
// inefficient as it is always generating the variables h, t and l4.
// With a bit more thought it would be possible to minimize
// the number of extra variables generated.
bool AppendClause2::apply_choice() {
  PVarPtr h = NEW_PVAR();
  PVarPtr t = NEW_PVAR();
  PVarPtr l4 = NEW_PVAR();
  if (!engine->unify(l1, make_open_list_from({h, t})))
    return false;
  if (!engine->unify(l3, make_open_list_from({h, l4})))
    return false;
  // Note that the following is doing the recursive call
  PredPtr app = std::make_shared<Append>(engine, t, l2, l4);
  app->set_continuation(continuation);
  continuation = app;
  return true;
}

// Print out all solutions
class PrintAndFail : public SemiDetPred {

public:
  PrintAndFail(Engine *eng, TermPtr arg1, TermPtr arg2, TermPtr arg3)
      : SemiDetPred(eng), l1(arg1), l2(arg2), l3(arg3) {}

  void initialize_call() override {
    std::cout << "Answer l1 = " << *(l1->dereference()) << " ";
    std::cout << "l2 = " << *(l2->dereference()) << " ";
    std::cout << "l3 = " << *(l3->dereference()) << std::endl;
    std::cout << std::endl;
  }

  bool apply_choice() override { return false; }

  bool test_choice() override { return false; }

private:
  TermPtr l1;
  TermPtr l2;
  TermPtr l3;
};