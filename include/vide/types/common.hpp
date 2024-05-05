#pragma once

#include <type_traits>
#include <utility>

#include <vide/binary_data.hpp>
#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for enum types
template <class Archive, class T>
	requires std::is_enum_v<T>
inline std::underlying_type_t<T> VIDE_FUNCTION_NAME_SAVE_MINIMAL(const Archive&, const T& var) {
	return std::to_underlying(var);
}

//! Loading for enum types
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_LOAD_MINIMAL(const Archive&, T& var, std::underlying_type_t<T> value) {
	var = T{value};
}

//! Serialization for raw pointers
/*! This exists only to throw a static_assert to let users know we don't support raw pointers. */
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive&, T*&) {
	static_assert(false, "Vide does not support serializing raw pointers - please use a smart pointer");
}

//! Serialization for C style arrays
template <class Archive, class T>
	requires std::is_array_v<T>
inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive& ar, T& array) {
	if constexpr (Archive::template supports_binary<std::remove_all_extents_t<T>>)
		ar(binary_data(array, sizeof(array)));
	else
		for (auto& i : array)
			ar(i);
}

} // namespace vide --------------------------------------------------------------------------------
