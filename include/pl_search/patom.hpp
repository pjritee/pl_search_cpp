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
#ifndef PL_SEARCH_PATOM_HPP
#define PL_SEARCH_PATOM_HPP

#include "term.hpp"
#include <string>
#include <typeinfo>

namespace pl_search {

// PAtom objects approximate Prolog atoms
class PAtom : public Term {
public:
  PAtom(const std::string &name) : name(name) {}

  Term *dereference() override { return this; }

  bool bind(Term *t) override {
    return false; // Atoms cannot be bound to other terms
  }

  void reset(Term *t) override {
    // No-op for PAtom
  }

  std::string getName() const { return name; }

  std::string repr() const override { return name; }

  bool isEqualTo(Term &t) override {
    PAtom *a = dynamic_cast<PAtom *>(&t);
    if (a == nullptr)
      return false;
    return name == a->name;
  }

  bool isLessThan(Term &t) override;

private:
  std::string name;
};

} // namespace pl_search
#endif // PL_SEARCH_PATOM_HPP