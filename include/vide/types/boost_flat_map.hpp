//

#pragma once

#include <vide/serializer.hpp>
#include <vide/types/concept_pair_associative_container.hpp>

#include <cstdint>
#include <boost/container/flat_map.hpp>


namespace vide { // --------------------------------------------------------------------------------

template <class K, class T, class C, class A>
struct serializer<boost::container::flat_map<K, T, C, A>> {
	template <class Archive>
	static void serialize(Archive& ar, boost::container::flat_map<K, T, C, A>& map, std::uint32_t) {
		if constexpr (Archive::is_output)
			::vide::implementation_map_save(ar, map);
		else
			::vide::implementation_map_load(ar, map);
	}
};

template <class K, class T, class C, class A>
struct serializer<boost::container::flat_multimap<K, T, C, A>> {
	template <class Archive>
	static void serialize(Archive& ar, boost::container::flat_multimap<K, T, C, A>& map, std::uint32_t) {
		if constexpr (Archive::is_output)
			::vide::implementation_map_save(ar, map);
		else
			::vide::implementation_map_load(ar, map);
	}
};

} // namespace vide --------------------------------------------------------------------------------
