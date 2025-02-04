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
#include "pint.hpp"
#include "pfloat.hpp"
#include "patom.hpp"
#include "clist.hpp"
#include "pvar.hpp"

bool PInt::isLessThan(Term& t) {
    if (typeid(t) == typeid(PVar)) return false;
    PInt* i = dynamic_cast<PInt*>(&t);
    if (i == nullptr) {
        PFloat* f = dynamic_cast<PFloat*>(&t);
        if (f == nullptr) return true;
        return value < f->getValue();
    }
    return value < i->value;
}

bool PFloat::isLessThan(Term& t) {
    if (typeid(t) == typeid(PVar)) return false;
    PFloat* f = dynamic_cast<PFloat*>(&t);
    if (f == nullptr) {
        PInt* i = dynamic_cast<PInt*>(&t);
        if (i == nullptr) return true;
        return value < i->getValue();
    }
    return value < f->value;
}

bool PAtom::isLessThan(Term& t) {
    if (typeid(t) == typeid(PVar)) return false;
    if (typeid(t) == typeid(PInt)) return false;
    if (typeid(t) == typeid(PFloat)) return false;
    PAtom* a = dynamic_cast<PAtom*>(&t);
    if (a == nullptr) return false;
    return name < a->name;
}

bool CList::isLessThan(Term& t) {
    CList* l = dynamic_cast<CList*>(&t);
    if (l == nullptr) return false;
    return elements < l->elements;
}