#pragma once

#include <array>

#include <vide/binary_data.hpp>
#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

template <class Archive, class T, std::size_t N>
inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive& ar, std::array<T, N>& array) {
	if constexpr (Archive::template supports_binary<T>) {
		ar(binary_data(array.data(), sizeof(array)));
	} else {
		for (auto& i : array)
			ar(i);
	}
}

} // namespace vide --------------------------------------------------------------------------------
