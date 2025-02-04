#ifndef TERM_HPP
#define TERM_HPP
#include <iostream>
#include <string>

using namespace std;

// Abstract base class for terms that approximate Prolog terms
class Term {
public:
  virtual Term* dereference() = 0;
  virtual bool bind(Term* t) = 0;
  virtual void reset() = 0;
  virtual std::string repr() const
	{
		return "TERM";
	}

  virtual bool isEqualTo(Term& t) = 0;
  virtual bool isLessThan(Term& t) = 0;

  Term() {
  }
  virtual ~Term() {
  } // Virtual destructor
  friend std::ostream& operator<<(std::ostream& os, const Term& t)
	{
  	os << t.repr();
		return os;
	}
 
  friend bool operator==( Term& t1, Term& t2 ) {
    return t1.isEqualTo(t2);
    }
  
  // Approximating the @< operator in Prolog
  // pvar < pint, pfloat < patom < clist < user-defined classes
  // "older" vars are less than "newer" vars
  // patoms are ordered by name
  // pints and pfloats are ordered by value
  // clists are ordered by the first element, then the second, etc.
  friend bool operator<( Term& t1, Term& t2 ) {
    return t1.isLessThan(t2);
  }

   friend bool operator<=( Term& t1, Term& t2 ) {
    return t1.isLessThan(t2) || t1.isEqualTo(t2);
   }
};



#endif
