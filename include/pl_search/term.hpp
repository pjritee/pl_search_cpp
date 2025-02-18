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
#ifndef PL_SEARCH_TERM_HPP
#define PL_SEARCH_TERM_HPP
#include <iostream>
#include <string>

// using namespace std;

namespace pl_search {

// Abstract base class for terms that approximate Prolog terms
class Term {
public:
  virtual Term *dereference() = 0;
  virtual bool bind(Term *t) = 0;
  virtual void reset(Term *t) = 0;
  virtual std::string repr() const { return "TERM"; }

  virtual bool isEqualTo(Term &t) = 0;
  virtual bool isLessThan(Term &t) = 0;

  // A hook for unification of user-defined classes
  // Only override this method if you want to unify user-defined classes
  bool unifyWith(Term *t) { return false; }

  virtual bool is_var() { return false; }

  Term() {}

  virtual ~Term() {} // Virtual destructor
  friend std::ostream &operator<<(std::ostream &os, const Term &t) {
    os << t.repr();
    return os;
  }

  friend bool operator==(Term &t1, Term &t2) {
    Term *deref1 = t1.dereference();
    Term *deref2 = t2.dereference();
    return deref1->isEqualTo(*deref2);
  }

  // Approximating the @< operator in Prolog
  // pvar < pint, pfloat < patom < clist < user-defined classes
  // "older" vars are less than "newer" vars
  // patoms are ordered by name
  // pints and pfloats are ordered by value
  // clists are ordered by the first element, then the second, etc.
  friend bool operator<(Term &t1, Term &t2) { return t1.isLessThan(t2); }

  friend bool operator<=(Term &t1, Term &t2) {
    return t1.isLessThan(t2) || t1.isEqualTo(t2);
  }
};

} // namespace pl_search

#endif // PL_SEARCH_TERM_HPP
