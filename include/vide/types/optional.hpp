#pragma once

#include <optional>

#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for std::optional
template <class Archive, typename T>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::optional<T>& optional) {
	ar.nvp("has_value", optional.has_value());
	if (optional)
		ar.nvp("value", *optional);
}

//! Loading for std::optional
template <class Archive, typename T>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::optional<T>& optional) {
	bool has_value;
	ar.nvp("has_value", has_value);

	if (has_value) {
		optional.emplace();
		ar.nvp("value", *optional);
	} else {
		optional = std::nullopt;
	}
}

} // namespace vide --------------------------------------------------------------------------------
