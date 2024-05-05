#pragma once

#include <set>

#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------
namespace set_detail {

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

	set.clear();

	auto hint = set.begin();
	for (typename Archive::size_type i = 0; i < size; ++i) {
		typename SetT::key_type key;

		ar(key);
		hint = set.emplace_hint(hint, std::move(key));
	}
}

} // namespace set_detail --------------------------------------------------------------------------

//! Saving for std::set
template <class Archive, class K, class C, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::set<K, C, A> const& set) {
	set_detail::save(ar, set);
}

//! Loading for std::set
template <class Archive, class K, class C, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::set<K, C, A>& set) {
	set_detail::load(ar, set);
}

//! Saving for std::multiset
template <class Archive, class K, class C, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::multiset<K, C, A> const& multiset) {
	set_detail::save(ar, multiset);
}

//! Loading for std::multiset
template <class Archive, class K, class C, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::multiset<K, C, A>& multiset) {
	set_detail::load(ar, multiset);
}

} // namespace vide --------------------------------------------------------------------------------
