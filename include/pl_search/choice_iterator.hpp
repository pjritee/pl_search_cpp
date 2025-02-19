#ifndef PL_SEARCH_CHOICE_ITERATOR_HPP
#define PL_SEARCH_CHOICE_ITERATOR_HPP

#include "engine.hpp"
#include <vector>

namespace pl_search {
class Engine;
class PVar;

// Base class for choice iterators
class ChoiceIterator {
public:
  virtual bool has_next() = 0;
  virtual bool make_choice() = 0;
  virtual ~ChoiceIterator() = default;
};

class VarChoiceIterator : public ChoiceIterator {
public:
  VarChoiceIterator(Engine *engine, PVar *v, std::vector<Term *> &ch)
      : engine(engine), var(v), choices(ch) {
    index = 0;
  }

  bool has_next() override { return index < choices.size(); }
  bool make_choice() override {
    Term *t = choices[index++];
    return engine->unify(var, t);
  }

private:
  Engine *engine;
  int index;
  PVar *var;
  std::vector<Term *> choices;
};

} // namespace pl_search

#endif // PL_SEARCH_CHOICE_ITERATOR_HPP