#pragma once

#include <utility>

#include <vide/macros.hpp>
#include <vide/nvp.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Serializing for std::pair
template <class Archive, class T1, class T2>
inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive& ar, std::pair<T1, T2>& pair) {
	ar(VIDE_NVP_("first", pair.first));
	ar(VIDE_NVP_("second", pair.second));
}

} // namespace vide --------------------------------------------------------------------------------
