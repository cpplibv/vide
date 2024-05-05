#pragma once

#include <stdexcept>
#include <string>


namespace vide {

// -------------------------------------------------------------------------------------------------

//! An exception class thrown when things go wrong at runtime
/*! @ingroup Utility */
struct Exception : public std::runtime_error {
	explicit Exception(const std::string& what_) : std::runtime_error(what_) {}
	explicit Exception(const char* what_) : std::runtime_error(what_) {}
};

// -------------------------------------------------------------------------------------------------

// //! An exception class thrown when continued deserialization would overallocate by requesting more memory than
// //! what is left unserialized in the raw data.
// /*! @ingroup Utility */
// struct OverallocationProtectionException : public Exception {
// 	explicit OverallocationProtectionException(const std::string& what_) : Exception(what_) {}
// 	explicit OverallocationProtectionException(const char* what_) : Exception(what_) {}
// };

// -------------------------------------------------------------------------------------------------

} // namespace vide
