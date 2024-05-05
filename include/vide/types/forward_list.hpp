#pragma once

#include <forward_list>

#include <vide/macros.hpp>

#include <ranges>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for std::forward_list all other types
template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::forward_list<T, A> const& forward_list) {
	// write the size - note that this is slow because we need to traverse
	// the entire list. there are ways we could avoid this but this was chosen
	// since it works in the most general fashion with any archive type
	ar.size_tag(static_cast<VIDE_SIZE_TYPE>(std::distance(forward_list.begin(), forward_list.end())));

	// write the list
	for (const auto& i : forward_list)
		ar(i);
}

//! Loading for std::forward_list all other types from
template <class Archive, class T, class A>
void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::forward_list<T, A>& forward_list) {
	const auto size = ar.size_tag();
	const auto reserveable = ar.template safe_to_reserve<T>(size);

	if (reserveable == size) {
		forward_list.resize(reserveable);
		for (auto& i : forward_list)
			ar(i);
	} else {
		auto gen = std::views::iota(0u, size) | std::views::transform([&](auto) {
			T value;
			ar(value);
			return value;
		});
		forward_list.assign(gen.begin(), gen.end());
	}
}

} // namespace vide --------------------------------------------------------------------------------
