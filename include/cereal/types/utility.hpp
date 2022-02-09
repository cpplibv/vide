#pragma once

#include <utility>

#include <cereal/macros.hpp>
#include <cereal/nvp.hpp>


namespace cereal {

// -------------------------------------------------------------------------------------------------

//! Serializing for std::pair
template <class Archive, class T1, class T2> inline
void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar, std::pair<T1, T2>& pair) {
	ar(CEREAL_NVP_("first", pair.first));
	ar(CEREAL_NVP_("second", pair.second));
}

// -------------------------------------------------------------------------------------------------

} // namespace cereal
