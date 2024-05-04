#pragma once

#include <vide/macros.hpp>
#include <vide/map_item.hpp>
#include <vide/size_tag.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for std-like pair associative containers
template <class Archive, template <typename...> class Map, typename... Args, typename = typename Map<Args...>::mapped_type>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, Map<Args...> const& map) {
	ar.size_tag(static_cast<size_type>(map.size()));

	for (const auto& i : map)
		ar(make_map_item(i.first, i.second));
}

//! Loading for std-like pair associative containers
template <class Archive, template <typename...> class Map, typename... Args, typename = typename Map<Args...>::mapped_type>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, Map<Args...>& map) {
	size_type size;
	ar.size_tag(size);

	map.clear();

	auto hint = map.begin();
	for (size_t i = 0; i < size; ++i) {
		typename Map<Args...>::key_type key;
		typename Map<Args...>::mapped_type value;

		ar(make_map_item(key, value));
		hint = map.emplace_hint(hint, std::move(key), std::move(value));
	}
}
} // namespace vide --------------------------------------------------------------------------------
