#pragma once

#include <vide/macros.hpp>
#include <vide/types/implementation_set.hpp>

#include <set>


namespace vide { // --------------------------------------------------------------------------------

/// Saving for std::set
template <class Archive, class K, class C, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::set<K, C, A>& set) {
	implementation_set_save(ar, set);
}

/// Loading for std::set
template <class Archive, class K, class C, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::set<K, C, A>& set) {
	implementation_set_load(ar, set);
}

/// Saving for std::multiset
template <class Archive, class K, class C, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::multiset<K, C, A>& multiset) {
	implementation_set_save(ar, multiset);
}

/// Loading for std::multiset
template <class Archive, class K, class C, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::multiset<K, C, A>& multiset) {
	implementation_set_load(ar, multiset);
}

} // namespace vide --------------------------------------------------------------------------------
