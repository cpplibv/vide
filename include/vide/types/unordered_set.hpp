#pragma once

#include <unordered_set>

#include <vide/macros.hpp>
#include <vide/size_tag.hpp>


namespace vide { // --------------------------------------------------------------------------------
namespace unordered_set_detail {

//! @internal
template <class Archive, class SetT>
inline void save(Archive& ar, SetT const& set) {
	ar.size_tag(static_cast<size_type>(set.size()));

	for (const auto& i : set)
		ar(i);
}

//! @internal
template <class Archive, class SetT>
inline void load(Archive& ar, SetT& set) {
	size_type size;
	ar.size_tag(size);

	set.clear();
	set.reserve(static_cast<std::size_t>(size));

	for (size_type i = 0; i < size; ++i) {
		typename SetT::key_type key;

		ar(key);
		set.emplace(std::move(key));
	}
}

} // namespace unordered_set_detail

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::unordered_set<K, H, KE, A> const& unordered_set) {
	unordered_set_detail::save(ar, unordered_set);
}

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::unordered_set<K, H, KE, A>& unordered_set) {
	unordered_set_detail::load(ar, unordered_set);
}

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::unordered_multiset<K, H, KE, A> const& unordered_multiset) {
	unordered_set_detail::save(ar, unordered_multiset);
}

template <class Archive, class K, class H, class KE, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::unordered_multiset<K, H, KE, A>& unordered_multiset) {
	unordered_set_detail::load(ar, unordered_multiset);
}

} // namespace vide --------------------------------------------------------------------------------
