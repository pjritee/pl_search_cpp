#ifndef PFLOAT_HPP
#define PFLOAT_HPP

#include "term.hpp"
#include <string>
#include <typeinfo>


// PFloat objects approximate Prolog floats
class PFloat : public Term {
public:
    PFloat(double value) : value(value) {}

    Term* dereference() override {
        return this;
    }

    bool bind(Term* t) override {
        return false; // Floats cannot be bound to other terms
    }

    void reset() override {
        // No-op for PFloat
    }

    std::string repr() const override {
        return std::to_string(value);
    }

    bool isEqualTo(Term& t) override {
        PFloat* f = dynamic_cast<PFloat*>(&t);
        if (f == nullptr) return false;
        return value == f->value;
    }

    bool isLessThan(Term& t) override;

    double getValue() const {
        return value;
    }

private:
    double value;
};

#endif // PFLOAT_HPP