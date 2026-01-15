#pragma once

#include <vide/macros.hpp>
#include <vide/types/implementation_unordered_set.hpp>

#include <unordered_set>


namespace vide { // --------------------------------------------------------------------------------

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::unordered_set<K, H, KE, A>& unordered_set) {
	implementation_unordered_set_save(ar, unordered_set);
}

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::unordered_set<K, H, KE, A>& unordered_set) {
	implementation_unordered_set_load(ar, unordered_set);
}

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::unordered_multiset<K, H, KE, A>& unordered_multiset) {
	implementation_unordered_set_save(ar, unordered_multiset);
}

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::unordered_multiset<K, H, KE, A>& unordered_multiset) {
	implementation_unordered_set_load(ar, unordered_multiset);
}

} // namespace vide --------------------------------------------------------------------------------
