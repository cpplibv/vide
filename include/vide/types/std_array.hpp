#pragma once

#include <vide/serializer.hpp>

#include <array>
#include <cstddef>
#include <cstdint>


namespace vide { // --------------------------------------------------------------------------------

template <class T, std::size_t N>
struct serializer<std::array<T, N>> {
	template <class Archive>
	static void serialize(Archive& ar, std::array<T, N>& array, std::uint32_t) {
		if constexpr (Archive::template supports_binary<T>) {
			ar.binary_data(array.data(), N);
		} else {
			for (auto& i : array)
				ar(i);
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
