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
#ifndef PL_SEARCH_CLIST_HPP
#define PL_SEARCH_CLIST_HPP

#include "term.hpp"
#include <list>
#include <string>
#include <sstream>

// CList objects replace Prolog lists for efficiency
// WARNING: Because the internal list is mutable care must be taken when using 
// CList objects. For example, it might be necessary to copy the list before
// passing it to a function that might modify it or when binding a variable to
// a CList object.
// If a Prolog-like list is required, a user-defined class that implements the
// Term interface should be used (for example ConsList).

namespace pl_search {


class CList : public Term {
public:
    CList() {}

    CList(const std::list<Term*>& elements) : elements(elements) {}

    Term* dereference() override {
        return this;
    }

    bool bind(Term* t) override {
        return false; // Lists cannot be bound to other terms
    }

    void reset(Term* t) override {
        // No-op for CList
    }

    std::string repr() const override {
        std::ostringstream oss;
        oss << "[";
        for (auto it = elements.begin(); it != elements.end(); ++it) {
            if (it != elements.begin()) {
                oss << ", ";
            }
            oss << (*it)->repr();
        }
        oss << "]";
        return oss.str();
    }

    bool isEqualTo(Term& t) override {
        CList* l = dynamic_cast<CList*>(&t);
        if (l == nullptr) return false;
        return elements == l->elements;
    }

    bool isLessThan(Term& t) override;

    void addElement(Term* element) {
        elements.push_back(element);
    }

    const std::list<Term*>& getElements() const {
        return elements;
    }

private:
    std::list<Term*> elements;
};

} // namespace pl_search

#endif // PL_SEARCH_CLIST_HPP