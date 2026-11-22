#pragma once

#include <vide/serializer.hpp>
#include <vide/types/concept_pair_associative_container.hpp>

#include <cstdint>
#include <unordered_map>


namespace vide { // --------------------------------------------------------------------------------

template <class K, class T, class H, class KE, class A>
struct serializer<std::unordered_map<K, T, H, KE, A>> {
	template <class Archive>
	static void serialize(Archive& ar, std::unordered_map<K, T, H, KE, A>& map, std::uint32_t) {
		if constexpr (Archive::is_output)
			::vide::implementation_map_save(ar, map);
		else
			::vide::implementation_map_load(ar, map);
	}
};

template <class K, class T, class H, class KE, class A>
struct serializer<std::unordered_multimap<K, T, H, KE, A>> {
	template <class Archive>
	static void serialize(Archive& ar, std::unordered_multimap<K, T, H, KE, A>& map, std::uint32_t) {
		if constexpr (Archive::is_output)
			::vide::implementation_map_save(ar, map);
		else
			::vide::implementation_map_load(ar, map);
	}
};

} // namespace vide --------------------------------------------------------------------------------
