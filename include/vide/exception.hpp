#pragma once

#include <stdexcept>


namespace vide {

// -------------------------------------------------------------------------------------------------

/// An exception class thrown when things go wrong at runtime
/// @ingroup Utility
struct Exception : public std::runtime_error {
	using std::runtime_error::runtime_error;
};

// -------------------------------------------------------------------------------------------------

// /// An exception class thrown when continued deserialization would overallocate by requesting more memory than
// /// what is left unserialized in the raw archive.
// /// @ingroup Utility
// struct OverallocationProtectionException : public Exception {
// 	explicit OverallocationProtectionException(const std::string& what_) : Exception(what_) {}
// 	explicit OverallocationProtectionException(const char* what_) : Exception(what_) {}
// };

// -------------------------------------------------------------------------------------------------

} // namespace vide
