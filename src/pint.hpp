#ifndef PINT_HPP
#define PINT_HPP

#include "term.hpp"
#include <string>
#include <typeinfo>


// PInt objects approximate Prolog integers
class PInt : public Term {
public:
    PInt(int value) : value(value) {}

    Term* dereference() override {
        return this;
    }

    bool bind(Term* t) override {
        return false; // Integers cannot be bound to other terms
    }

    void reset() override {
        // No-op for PInt
    }

    std::string repr() const override {
        return std::to_string(value);
    }

    bool isEqualTo(Term& t) override {
        PInt* i = dynamic_cast<PInt*>(&t);
        if (i == nullptr) return false;
        return value == i->value;
    }

    bool isLessThan(Term& t) override;

    int getValue() const {
        return value;
    }

private:
    int value;
};

#endif // PINT_HPP