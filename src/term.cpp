#include "term.hpp"

using namespace std;


std::ostream& operator<<( std::ostream& os, const Term& t ){
  switch (t.kind) {
  case VAR:
    os << "X" << t.var_id;
    break;
  case INT:
    os << t.int_val;
    break;
  case LIST:
    os << "LIST";
    break;
  }
  return os;
}

bool operator==( Term& t1, Term& t2 ){
  if (t1.kind != t2.kind) return false;

  switch (t1.kind) {
  case VAR:
   return t1.var_id == t2.var_id;
    break;
  case INT:
    return t1.int_val == t2.int_val;
    break;
  case LIST:
    return false;
    break;
  }
  return false;
}
bool operator<=( Term& t1, Term& t2 ){
  switch (t1.kind) {
  case VAR:
    if (t2.kind == LIST) return true;
    if (t2.kind == INT) return false;
    return (t1.var_id <= t2.var_id);
    break;
  case INT:
    if (t2.kind != INT) return true;
    return t1.int_val <= t2.int_val;
    break;
  case LIST:
    return false;
    break;
  }
  return false;
}

bool operator<( Term& t1, Term& t2 ){
  return (t1 <= t2) && !(t1 == t2);
}


Term* Term::dereference() {
  Term* result = this;
  
  while (true) {
    switch (result->kind) {
    case VAR:
      if ( result == result->term_ptr) return result;
      result = result->term_ptr;
      break;
    case INT:
    case LIST:
      return result;
      break;
    }
  }
  return result;
}

void Term::bind(Term* t) {
  if (kind != VAR) return;
  term_ptr = t;
}

void Term::bind(TermList& t) {
  if (kind != VAR) return;
  list_val = t;
  kind = LIST;
}

void Term::reset() {
  if (kind == INT) return;
  kind = VAR;
  term_ptr = this;
  list_val.clear();
}

std::ostream& operator<<( std::ostream& os, const TermList& t ) {
  for (auto it = t.begin(); it != t.end(); it++) {
    os << **it;
  }
  return os;
}

int Term::id = 0;



// int main() {
//   Term v1 = Term();
//   Term v2 = Term();
//   Term n = Term(42);
//   Term z = Term(0);
  
//   cout << v1 << " " << v2 << n <<endl;

//   list<Term*> tl = list<Term*>({&v2,&n,&z});

//   v1.bind(&n);

//   Term* d = v1.dereference();

//   cout << "D " << d->kind<<  " "<< *d  << endl;
//   //tl.sort();

//   for (auto val :d->list_val) cout << *val << ", ";
//   cout << endl;
//   v1.reset();
//   d = v1.dereference();
//   cout << "D " << d->kind <<  " " << *d << endl;
//   tl.clear();
//   cout << "XXX" << endl;
//   return 1;
// }

