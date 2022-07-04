#pragma once

#include <optional>

#include <vide/macros.hpp>
#include <vide/nvp.hpp>


namespace vide {

// -------------------------------------------------------------------------------------------------

//! Saving for std::optional
template <class Archive, typename T>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::optional<T>& optional) {
	if (!optional) {
		ar(VIDE_NVP_("nullopt", true));
	} else {
		ar(VIDE_NVP_("nullopt", false));
		ar(VIDE_NVP_("data", *optional));
	}
}

//! Loading for std::optional
template <class Archive, typename T>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::optional<T>& optional) {
	bool nullopt;
	ar(VIDE_NVP_("nullopt", nullopt));

	if (nullopt) {
		optional = std::nullopt;
	} else {
		optional.emplace();
		ar(VIDE_NVP_("data", *optional));
	}
}

// -------------------------------------------------------------------------------------------------

} // namespace vide
