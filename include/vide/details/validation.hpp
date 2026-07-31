//

#pragma once

#include <vide/exception.hpp>

#include <ranges>
#include <type_traits>


namespace vide { // ----------------------------------------------------------------------------------------------------

struct notnull_t {
	template <typename T>
	inline void operator()(const T& var) const {
		if constexpr (std::ranges::range<T> && !std::is_convertible_v<T, bool>) {
			for (const auto& item : var)
				if (!item)
					throw vide::Exception("Non-null validation failed during serialization: ranges element is null.");
		} else {
			if (!var)
				throw vide::Exception("Non-null validation failed during serialization: object is null.");
		}
	}
};

struct notnullrange_t {
	template <typename T>
	inline void operator()(const T& var) const {
		for (const auto& item : var)
			if (!item)
				throw vide::Exception("Non-null validation failed during serialization: ranges element is null.");
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
			throw vide::Exception("Max size validation failed during serialization: object size exceeds the maximum limit.");
	}
};

struct minsize_t {
	std::size_t limit = 0;

	template <typename T>
	inline void operator()(const T& var) const {
		if (var.size() < limit)
			throw vide::Exception("Min size validation failed during serialization: object size does not reach the minimum limit.");
	}
};

template <typename Base>
struct indirect_t : private Base {
	template <typename... Args>
	explicit constexpr inline indirect_t(Args&&... args) :
		Base(std::forward<Args>(args)...) {
	}

	template <typename T>
	inline void operator()(const T& var) const {
		if (var)
			Base::operator()(*var);
	}
};

template <typename Base>
struct ranged_t : private Base {
	template <typename... Args>
	explicit constexpr inline ranged_t(Args&&... args) :
		Base(std::forward<Args>(args)...) {
	}

	template <typename T>
	inline void operator()(const T& var) const {
		for (const auto& item : var)
			Base::operator()(item);
	}
};

// ---------------------------------------------------------------------------------------------------------------------

constexpr inline notnull_t notnull;
constexpr inline notnullrange_t notnullrange;
constexpr inline notempty_t notempty;

[[nodiscard]] constexpr inline maxsize_t maxsize(std::size_t limit) {
	return maxsize_t{limit};
}

[[nodiscard]] constexpr inline minsize_t minsize(std::size_t limit) {
	return minsize_t{limit};
}

template <typename Base>
[[nodiscard]] constexpr inline auto indirect(Base&& baseValidator) {
	return indirect_t<std::remove_cvref_t<Base>>{std::forward<Base>(baseValidator)};
}

template <typename Base>
[[nodiscard]] constexpr inline auto ranged(Base&& baseValidator) {
	return ranged_t<std::remove_cvref_t<Base>>{std::forward<Base>(baseValidator)};
}

[[nodiscard]] constexpr inline auto indirect_maxsize(std::size_t limit) {
	return indirect_t<maxsize_t>{limit};
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
