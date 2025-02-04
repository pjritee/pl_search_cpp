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