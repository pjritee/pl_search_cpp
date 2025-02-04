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
#ifndef PRED_HPP_
#define PRED_HPP_
#include "term.hpp"
#include "engine.hpp"
#include <stack>
#include <memory>

class Engine;

class Pred;

typedef shared_ptr<Pred> PredPtr;

class Pred {
public:
  Engine* engine;
  PredPtr continuation;
  bool call();
  bool try_call();
  virtual void initialize_call();
  virtual bool apply_choice();
  virtual bool test_choice();
  virtual bool more_choices();
  virtual PredPtr get_continuation();

  Pred(Engine* eng, PredPtr cont);
};


class ChoicePred : public Pred {
 public:
  Term* var;
  stack<Term*> choices;
 
  void initialize_call();
  bool apply_choice();
  bool test_choice();
  bool more_choices();
  PredPtr get_continuation();
 
 ChoicePred(Engine* eng, PredPtr cont, Term* v,
            stack<Term*> &ch);
 
};



#endif
