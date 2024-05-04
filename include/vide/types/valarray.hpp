#pragma once

#include <valarray>

#include <vide/binary_data.hpp>
#include <vide/macros.hpp>
#include <vide/size_tag.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for std::valarray all other types
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::valarray<T>& valarray) {
	// TODO P1: Switch to a more generic concept from is_arithmetic_v to determine if a type is binary serializable
	constexpr bool binary_serializable = std::is_arithmetic_v<T>;
	constexpr bool serialize_as_binary = binary_serializable && Archive::template supports_type<BinaryData<T>>;

	ar.size_tag(static_cast<size_type>(valarray.size())); // number of elements

	if constexpr (serialize_as_binary) {
		ar(binary_data(&valarray[0], valarray.size() * sizeof(T)));
	} else {
		for (const auto& i : valarray)
			ar(i);
	}
}

//! Loading for std::valarray all other types
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::valarray<T>& valarray) {
	// TODO P1: Switch to a more generic concept from is_arithmetic_v to determine if a type is binary serializable
	constexpr bool binary_serializable = std::is_arithmetic_v<T>;
	constexpr bool serialize_as_binary = binary_serializable && Archive::template supports_type<BinaryData<T>>;

	size_type valarraySize;
	ar.size_tag(valarraySize);

	valarray.resize(static_cast<size_t>(valarraySize));

	if constexpr (serialize_as_binary) {
		ar(binary_data(&valarray[0], static_cast<std::size_t>(valarraySize) * sizeof(T)));
	} else {
		for (auto&& i : valarray)
			ar(i);
	}
}

} // namespace vide --------------------------------------------------------------------------------
