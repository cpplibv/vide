#pragma once

#include <vide/macros.hpp>
#include <vide/map_item.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for std-like pair associative containers, concept is on typename Map::mapped_type
template <typename Archive, typename Map>
	requires requires { typename Map::mapped_type; }
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const Map& map) {
	ar.size_tag(map.size());

	for (const auto& i : map)
		ar(make_map_item(i.first, i.second));
}

//! Loading for std-like pair associative containers, concept is on typename Map::mapped_type
template <typename Archive, typename Map>
	requires requires { typename Map::mapped_type; }
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, Map& map) {
	const auto size = ar.size_tag();
	const auto reserveable = ar.template safe_to_reserve<typename Map::value_type>(size);

	map.clear();
	if constexpr (requires { map.reserve(reserveable); })
		// std::map / std::multimap has no reserve
		// std::unordered_map / std::unordered_multimap has reserve
		map.reserve(reserveable);

	auto hint = map.begin();
	for (size_t i = 0; i < size; ++i) {
		typename Map::key_type key;
		typename Map::mapped_type value;

		ar(make_map_item(key, value));
		hint = map.emplace_hint(hint, std::move(key), std::move(value));
	}
}

} // namespace vide --------------------------------------------------------------------------------
