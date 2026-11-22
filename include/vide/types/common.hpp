#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <vide/binary_data.hpp>
#include <vide/serializer.hpp>


namespace vide { // --------------------------------------------------------------------------------

/// Serialization for raw pointers
/*! This exists only to throw a static_assert to let users know we don't support raw pointers. */
template <class T>
struct serializer<T*> {
	template <class Archive>
	static void serialize(Archive&, T*&, std::uint32_t) {
		static_assert(false, "Vide does not support serializing raw pointers - please use a smart pointer");
	}
};

/// Serialization for C style arrays
template <class T, std::size_t N>
struct serializer<T[N]> {
	template <class Archive>
	static void serialize(Archive& ar, T (&array)[N], std::uint32_t) {
		if constexpr (Archive::template supports_binary<std::remove_all_extents_t<T>>)
			ar(binary_data(array, sizeof(array)));
		else
			for (auto& i : array)
				ar(i);
	}
};

} // namespace vide --------------------------------------------------------------------------------
