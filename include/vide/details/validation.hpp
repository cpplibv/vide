//

#pragma once

#include <vide/exception.hpp>


namespace vide { // ----------------------------------------------------------------------------------------------------

struct notnull_t {
	template <typename T>
	inline void operator()(const T& var) const {
		if (!var)
			throw vide::Exception("Non-null validation failed during serialization: object is null.");
	}
};

struct notempty_t {
	template <typename T>
	inline void operator()(const T& var) const {
		if (var.empty())
			throw vide::Exception("Non-empty validation failed during serialization: object is empty.");
	}
};

struct maxsize_t {
	std::size_t limit = 0;

	template <typename T>
	inline void operator()(const T& var) const {
		if (var.size() > limit)
			throw vide::Exception("Max size validation failed during serialization: object size exceeds the limit.");
	}
};

// ---------------------------------------------------------------------------------------------------------------------

constexpr inline notnull_t notnull;
constexpr inline notempty_t notempty;

[[nodiscard]] constexpr inline maxsize_t maxsize(std::size_t limit) {
	return maxsize_t{limit};
}

} // namespace vide ----------------------------------------------------------------------------------------------------

// Other considered approaches:
//
// Preferred ideas:
// 	- standalone / simple check after member
//  		ar.verify(version >= 3);
// 		ar.verify(version >= 3, "older versions are not supported");
// 		ar.nvp("taxRate", taxRate);
// 		ar.verify(taxRate > 0 && taxRate < 100, "taxRate value");
// 		ar.verify(ptr);
// 		ar.verifyNotNull(ptr);
//
// 	- tag type
// 		ar.nvp("planet", planet, ar.notnull)
//
// Possible ideas:
// 	- type wrapper
// 		ar.nvp("planet", ar.notnull(planet))
// 		ar.nvp("planet", vide::notnull(planet))
//
// Rejected ideas:
// 	- chaining on a member (conflicts with current ret value)
// 		ar.nvp("taxRate", taxRate)
//  				.verify(taxRate > 0 && taxRate < 100, "taxRate value");
// 		ar.nvp("planet", planet)
// 				.verifyNotNull();
//
// 	- prefix chain
// 		ar.notnull().nvp("planet", planet);
