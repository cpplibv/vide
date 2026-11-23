//

#pragma once

#include <vide/types/implementation_set.hpp>
#include <vide/serializer.hpp>

#include <boost/container/flat_set.hpp>

#include <cstdint>


namespace vide { // --------------------------------------------------------------------------------

template <class K, class C, class A>
struct serializer<boost::container::flat_set<K, C, A>> {
	template <class Archive>
	static void serialize(Archive& ar, boost::container::flat_set<K, C, A>& set, std::uint32_t) {
		if constexpr (Archive::is_output)
			::vide::implementation_set_save(ar, set);
		else
			::vide::implementation_set_load(ar, set);
	}
};

template <class K, class C, class A>
struct serializer<boost::container::flat_multiset<K, C, A>> {
	template <class Archive>
	static void serialize(Archive& ar, boost::container::flat_multiset<K, C, A>& multiset, std::uint32_t) {
		if constexpr (Archive::is_output)
			::vide::implementation_set_save(ar, multiset);
		else
			::vide::implementation_set_load(ar, multiset);
	}
};

} // namespace vide --------------------------------------------------------------------------------
