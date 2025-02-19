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

/**
 * @brief Typedef for a shared pointer to a Pred object.
 */
typedef std::shared_ptr<Pred> PredPtr;

} // namespace pl_search

#endif // PL_SEARCH_TYPEDEFS_HPP