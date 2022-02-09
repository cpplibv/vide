#pragma once

#include <optional>

#include <cereal/macros.hpp>
#include <cereal/nvp.hpp>


namespace cereal {

// -------------------------------------------------------------------------------------------------

//! Saving for std::optional
template <class Archive, typename T>
inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const std::optional<T>& optional) {
	if (!optional) {
		ar(CEREAL_NVP_("nullopt", true));
	} else {
		ar(CEREAL_NVP_("nullopt", false));
		ar(CEREAL_NVP_("data", *optional));
	}
}

//! Loading for std::optional
template <class Archive, typename T>
inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, std::optional<T>& optional) {
	bool nullopt;
	ar(CEREAL_NVP_("nullopt", nullopt));

	if (nullopt) {
		optional = std::nullopt;
	} else {
		optional.emplace();
		ar(CEREAL_NVP_("data", *optional));
	}
}

// -------------------------------------------------------------------------------------------------

} // namespace cereal
