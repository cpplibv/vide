//

#pragma once

#include <vide/exception.hpp>
#include <vide/map_item.hpp>


namespace vide { // --------------------------------------------------------------------------------

/// Shared implementation of the std-like pair associative container serializers
template <typename Archive, typename Map>
inline void implementation_map_save(Archive& ar, const Map& map) {
	ar.size_tag(map.size());

	for (const auto& i : map)
		ar(MapItem{i.first, i.second});
}

template <typename Archive, typename Map>
inline void implementation_map_load(Archive& ar, Map& map) {
	const auto size = ar.size_tag();
	const auto reservable = ar.template safe_to_reserve<typename Map::value_type>(size);

	map.clear();
	if constexpr (requires { map.reserve(reservable); })
		// std::map / std::multimap has no reserve
		// std::unordered_map / std::unordered_multimap has reserve
		map.reserve(reservable);

	for (size_t i = 0; i < size; ++i) {
		typename Map::key_type key;
		typename Map::mapped_type value;

		ar(MapItem{key, value});
		// Hint at the end is mandatory to preserve the insertion order for multiset
		map.emplace_hint(map.end(), std::move(key), std::move(value));
	}

	if (map.size() != size)
		throw Exception("Failed to load container with unique key constraint. Found " + std::to_string(size - map.size()) + " duplicate key.");
}

} // namespace vide --------------------------------------------------------------------------------
