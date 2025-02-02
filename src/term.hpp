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
    term_ptr = this;#ifndef TERM_HPP
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
