#pragma once

#include <utility>

#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Serializing for std::pair
template <class Archive, class T1, class T2>
inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive& ar, std::pair<T1, T2>& pair) {
	ar.nvp("first", pair.first);
	ar.nvp("second", pair.second);
}

} // namespace vide --------------------------------------------------------------------------------
