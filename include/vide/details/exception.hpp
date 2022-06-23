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

} // namespace vide
