/*
MIT License

...license text...

*/

/**
 * @file typedefs.hpp
 * @brief Definition of common typedefs used in the PLSearchCPP library.
 */

#ifndef PL_SEARCH_TYPEDEFS_HPP
#define PL_SEARCH_TYPEDEFS_HPP

#include <memory>

namespace pl_search {

class Pred;

class Term;
class PVar;
class PInt;
class PFloat;
class PAtom;
class CList;
class UpdatablePVar;

/**
 * @brief Typedef for a shared pointer to a Pred object.
 */
typedef std::shared_ptr<Pred> PredPtr;

/**
 * @brief Helper function to create a shared pointer to a Pred object.
 */
#define NewPred std::make_shared<Pred>

/**
 * @brief Typedefs for shared pointers to Terms and subclasses.
 */
typedef std::shared_ptr<Term> TermPtr;
typedef std::shared_ptr<PVar> PVarPtr;
typedef std::shared_ptr<PInt> PIntPtr;
typedef std::shared_ptr<PFloat> PFloatPtr;
typedef std::shared_ptr<PAtom> PAtomPtr;
typedef std::shared_ptr<CList> CListPtr;
typedef std::shared_ptr<UpdatablePVar> UpdatablePVarPtr;

/**
 * @brief Helper functions to create shared pointers to Terms and subclasses.
 */
#define NewPVar std::make_shared<PVar>
#define NewPInt std::make_shared<PInt>
#define NewPFloat std::make_shared<PFloat>
#define NewPAtom std::make_shared<PAtom>
#define NewCList std::make_shared<CList>
#define NewUpdatablePVar std::make_shared<UpdatablePVar>

} // namespace pl_search

#endif // PL_SEARCH_TYPEDEFS_HPP