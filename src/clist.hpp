#ifndef CLIST_HPP
#define CLIST_HPP

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

    void reset() override {
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

#endif // CLIST_HPP