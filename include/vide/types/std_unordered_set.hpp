#pragma once

#include <vide/exception.hpp>
#include <vide/macros.hpp>

#include <unordered_set>


namespace vide { // --------------------------------------------------------------------------------
namespace unordered_set_detail {

//! @internal
template <class Archive, class SetT>
inline void save(Archive& ar, SetT const& set) {
	ar.size_tag(set.size());

	for (const auto& i : set)
		ar(i);
}

//! @internal
template <class Archive, class SetT>
inline void load(Archive& ar, SetT& set) {
	const auto size = ar.size_tag();
	const auto reservable = ar.template safe_to_reserve<typename SetT::value_type>(size);

	set.clear();
	set.reserve(reservable);

	for (typename Archive::size_type i = 0; i < size; ++i) {
		typename SetT::key_type key;

		ar(key);
		set.emplace(std::move(key));
	}

	if (set.size() != size)
		throw Exception("Failed to load container with unique key constraint. Found " + std::to_string(size - set.size()) + " duplicate key.");
}

} // namespace unordered_set_detail ----------------------------------------------------------------

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::unordered_set<K, H, KE, A>& unordered_set) {
	unordered_set_detail::save(ar, unordered_set);
}

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::unordered_set<K, H, KE, A>& unordered_set) {
	unordered_set_detail::load(ar, unordered_set);
}

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::unordered_multiset<K, H, KE, A>& unordered_multiset) {
	unordered_set_detail::save(ar, unordered_multiset);
}

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::unordered_multiset<K, H, KE, A>& unordered_multiset) {
	unordered_set_detail::load(ar, unordered_multiset);
}

} // namespace vide --------------------------------------------------------------------------------
