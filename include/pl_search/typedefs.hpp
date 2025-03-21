/*
MIT License

...license text...

*/

/**
 * @file typedefs.hpp
 * @brief Definition of common typedefs used in the pl_search library.
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
#define NEW_PRED std::make_shared<Pred>

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
#define NEW_PVAR std::make_shared<PVar>
#define NEW_PINT std::make_shared<PInt>
#define NEW_PFLOAT std::make_shared<PFloat>
#define NEW_PATOM std::make_shared<PAtom>
#define NEW_CLIST std::make_shared<CList>
#define NEW_UPDATABLE_PVAR std::make_shared<UpdatablePVar>

} // namespace pl_search

#endif // PL_SEARCH_TYPEDEFS_HPP