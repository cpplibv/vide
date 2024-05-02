#pragma once

#include <array>

#include <vide/binary_data.hpp>
#include <vide/macros.hpp>


namespace vide {

// -------------------------------------------------------------------------------------------------

template <class Archive, class T, std::size_t N>
inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive& ar, std::array<T, N>& array) {
	// TODO P1: Switch to a more generic concept from is_arithmetic_v to determine if a type is binary serializable
	constexpr bool binary_serializable = std::is_arithmetic_v<T>;
	constexpr bool serialize_as_binary = binary_serializable && Archive::template could_serialize<BinaryData<T>>;

	if constexpr (serialize_as_binary) {
		ar(binary_data(array.data(), sizeof(array)));

	} else {
		for (auto& i : array)
			ar(i);
	}
}

// -------------------------------------------------------------------------------------------------

} // namespace vide
