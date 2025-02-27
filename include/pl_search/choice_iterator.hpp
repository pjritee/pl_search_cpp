/*
MIT License

...license text...

*/

#ifndef PL_SEARCH_CHOICE_ITERATOR_HPP
#define PL_SEARCH_CHOICE_ITERATOR_HPP

#include "engine.hpp"
#include "typedefs.hpp"

#include <vector>

namespace pl_search {
class Engine;
class PVar;

/**
 * @brief Base class for choice iterators.
 */
class ChoiceIterator {
public:
  /**
   * @brief Checks if there are more choices available.
   * @return True if there are more choices, false otherwise.
   */
  virtual bool has_next() = 0;

  /**
   * @brief Makes a choice.
   * @return True if the choice is made successfully, false otherwise.
   */
  virtual bool make_choice() = 0;

  /**
   * @brief Tests a choice which would tyically involve constraint dedections
   * based on the choice.
   * @return True if the choice is valid, false otherwise.
   */
  virtual bool test_choice() { return true; }

  /**
   * @brief Virtual destructor for proper cleanup.
   */
  virtual ~ChoiceIterator() = default;
};

/**
 * @brief Choice iterator for variables.
 */
class VarChoiceIterator : public ChoiceIterator {
public:
  /**
   * @brief Constructs a VarChoiceIterator.
   * @param engine Pointer to the Engine.
   * @param v Pointer to the variable.
   * @param ch Reference to the vector of choices.
   */
  VarChoiceIterator(Engine *engine, PVarPtr v, std::vector<TermPtr> ch)
      : engine(engine), var(v), choices(ch), index(0) {}

  /**
   * @brief Checks if there are more choices available.
   * @return True if there are more choices, false otherwise.
   */
  bool has_next() override { return index < choices.size(); }

  /**
   * @brief Makes a choice.
   * @return True if the choice is made successfully and follow up
   * deductions do not fail, false otherwise.
   */
  virtual bool make_choice() override {
    TermPtr t = choices[index++];
    return engine->unify(var, t) && test_choice();
  }

protected:
  Engine *engine;               ///< Pointer to the Engine.
  int index;                    ///< Current index in the choices vector.
  PVarPtr var;                  ///< Pointer to the variable.
  std::vector<TermPtr> choices; ///< Reference to the vector of choices.
};

} // namespace pl_search

#endif // PL_SEARCH_CHOICE_ITERATOR_HPP