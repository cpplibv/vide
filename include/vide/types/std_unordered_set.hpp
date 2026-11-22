#pragma once

#include <vide/types/implementation_unordered_set.hpp>
#include <vide/serializer.hpp>

#include <cstdint>
#include <unordered_set>


namespace vide { // --------------------------------------------------------------------------------

template <class K, class H, class KE, class A>
struct serializer<std::unordered_set<K, H, KE, A>> {
	template <class Archive>
	static void serialize(Archive& ar, std::unordered_set<K, H, KE, A>& unordered_set, std::uint32_t) {
		if constexpr (Archive::is_output) {
			::vide::implementation_unordered_set_save(ar, unordered_set);
		} else {
			::vide::implementation_unordered_set_load(ar, unordered_set);
		}
	}
};

template <class K, class H, class KE, class A>
struct serializer<std::unordered_multiset<K, H, KE, A>> {
	template <class Archive>
	static void serialize(Archive& ar, std::unordered_multiset<K, H, KE, A>& unordered_multiset, std::uint32_t) {
		if constexpr (Archive::is_output) {
			::vide::implementation_unordered_set_save(ar, unordered_multiset);
		} else {
			::vide::implementation_unordered_set_load(ar, unordered_multiset);
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
