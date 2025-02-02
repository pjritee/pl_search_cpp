#ifndef TERM_HPP
#define TERM_HPP

#include <ostream>
#include <iostream>
#include <sstream>
#include <list>

using namespace std;

enum Kind {VAR, INT, LIST};

class Term;

typedef list<Term*> TermList;

class Term {
public:
  static int id; 

  Term* term_ptr;
  int var_id;
  int int_val;
  TermList list_val;
  Kind  kind;

  Term* dereference();

  void bind(Term* t);
  void bind(TermList& t);
  void reset();
  
  Term() {
    var_id = ++id;
    term_ptr = this;
    int_val = -99;
    kind = VAR;
    //cout << "Create var" << endl;
  }

  Term(int n) {
    var_id = -999;
    term_ptr = this;
    int_val = n;
    kind = INT;
    //cout << "Create int " << n << endl;
  }

  ~Term() {
    //cout << "destroy " << kind << endl;
  }
  
  friend std::ostream& operator<<( std::ostream& os, const Term& t );

  friend bool operator==( Term& t1, Term& t2 );
  friend bool operator<=( Term& t1, Term& t2 );
  friend bool operator<( Term& t1, Term& t2 );
};

std::ostream& operator<<( std::ostream& os, const TermList& t );



#endif
