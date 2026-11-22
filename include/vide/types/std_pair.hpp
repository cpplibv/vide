#pragma once

#include <cstdint>
#include <utility>

#include <vide/serializer.hpp>


namespace vide { // --------------------------------------------------------------------------------

template <class T1, class T2>
struct serializer<std::pair<T1, T2>> {
	template <class Archive>
	static void serialize(Archive& ar, std::pair<T1, T2>& pair, std::uint32_t) {
		ar.nvp("first", pair.first);
		ar.nvp("second", pair.second);
	}
};

} // namespace vide --------------------------------------------------------------------------------
