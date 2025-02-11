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
#ifndef PL_SEARCH_ENGINE_HPP_
#define PL_SEARCH_ENGINE_HPP_

#include "term.hpp"
//#include "pred.hpp"
#include "pvar.hpp"
#include "clist.hpp"
#include "typedefs.hpp"

#include <stack>
#include <memory>

namespace pl_search {


struct trail_entry {
  PVar* var;
  Term* value;
};


struct env_entry {
  PredPtr pred;
  int trail_index;
};

class Engine {
public:

  Engine() {};

  std::stack<std::shared_ptr<trail_entry>> trail_stack;

  std::stack<std::shared_ptr<env_entry>> env_stack;
  
  void trail(PVar* v);

  void backtrack();

  bool unify(Term* v, Term* t);

  bool execute(PredPtr p, bool unbind);

  bool push_and_call(PredPtr p);

  void push(PredPtr p);

  void pop_call();

  void pop_to_once();

  void clear_stacks();
};

} // namespace pl_search
#endif // PL_SEARCH_ENGINE_HPP_
