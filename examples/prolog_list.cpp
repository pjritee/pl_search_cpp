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

// This example has two parts:
//
// 1. Prolog lists as an example of user defined term - see prolog_list.hpp
//
// 2. Defining the equivalent of the Prolog append predicate -
//    see append_pred.hpp
//
// This program gives some simple examples of using the above.
//

#include "prolog_list.hpp"
#include "append_pred.hpp"
#include "pl_search/pint.hpp"

using namespace pl_search;

int main() {
  PIntPtr n1 = NEW_PINT(1);
  PIntPtr n2 = NEW_PINT(2);
  PIntPtr n3 = NEW_PINT(3);
  PIntPtr n4 = NEW_PINT(4);
  std::cout << "build and print a list" << std::endl;
  PrologListPtr lst = make_list_from({n1, n2, n3});
  std::cout << *lst << std::endl;

  std::cout << "build and print an open list" << std::endl;
  PrologListPtr open_lst = make_open_list_from({n1, n2, NEW_PVAR()});
  std::cout << *open_lst << std::endl;

  std::cout << "unify two lists" << std::endl;
  Engine engine;
  PrologListPtr l1 = make_list_from({n1, NEW_PVAR(), n2, n3});
  PrologListPtr l2 = make_open_list_from({NEW_PVAR(), NEW_PVAR(), NEW_PVAR()});
  std::cout << "Before unification: l1 = " << *l1 << " l2 = " << *l2
            << std::endl;
  if (!engine.unify(l1, l2)) {
    std::cout << "Unify fails" << std::endl;
    return 1;
  }
  std::cout << "After unification: l1 = " << *l1 << " l2 = " << *l2
            << std::endl;

  // The equivalent of the Prolog query  append([1,2], [3,4], L3)
  l1 = make_list_from({n1, n2});
  l2 = make_list_from({n3, n4});
  PVarPtr l3var = NEW_PVAR();
  std::cout << std::endl << "Test append in the forward direction" << std::endl;
  std::cout << std::endl << "Before append call ";
  std::cout << "l1 = " << *l1 << " ";
  std::cout << "l2 = " << *l2 << " ";
  std::cout << "l3var = " << *l3var << std::endl;
  std::cout << std::endl;
  std::cout << "Solutions after call" << std::endl << std::endl;
  std::shared_ptr<Pred> print_fail =
      std::make_shared<PrintAndFail>(&engine, l1, l2, l3var);

  engine.execute(conjunction({std::make_shared<Append>(&engine, l1, l2, l3var),
                              print_fail}),
                 false);

  std::cout << std::endl << "End of test" << std::endl << std::endl;

  // The equivalent of the Prolog query  append(L1, L2, [1,2,3,4])
  PVarPtr l1var = NEW_PVAR();
  PVarPtr l2var = NEW_PVAR();
  PrologListPtr l3 = make_list_from({n1, n2, n3, n4});
  std::cout << std::endl
            << "Test append in the backward direction" << std::endl;

  std::cout << std::endl << "Before append call ";
  std::cout << "l1 = " << *l1var << " ";
  std::cout << "l2 = " << *l2var << " ";
  std::cout << "l3 = " << *l3var << std::endl;
  std::cout << std::endl;
  std::cout << "Solutions after call" << std::endl << std::endl;
  print_fail = std::make_shared<PrintAndFail>(&engine, l1var, l2var, l3);

  engine.execute(
      conjunction(
          {std::make_shared<Append>(&engine, l1var, l2var, l3), print_fail}),
      false);

  std::cout << std::endl << "End of test" << std::endl << std::endl;
  return 0;
}
