#ifndef PATOM_HPP
#define PATOM_HPP

#include "term.hpp"
#include <string>
#include <typeinfo>


// PAtom objects approximate Prolog atoms
class PAtom : public Term {
public:
    PAtom(const std::string& name) : name(name) {}

    Term* dereference() override {
        return this;
    }

    bool bind(Term* t) override {
        return false; // Atoms cannot be bound to other terms
    }

    void reset() override {
        // No-op for PAtom
    }

    std::string getName() const {
        return name;
    }

    std::string repr() const override {
       return name;
    }

    bool isEqualTo(Term& t) override {
        PAtom* a = dynamic_cast<PAtom*>(&t);
        if (a == nullptr) return false;
        return name == a->name;
    }

    bool isLessThan(Term& t) override;

private:
    std::string name;
};

#endif // PATOM_HPP