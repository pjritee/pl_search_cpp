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

    void reset(Term* t) override {
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