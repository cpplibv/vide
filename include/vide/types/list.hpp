#pragma once

#include <list>

#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for std::list
template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::list<T, A>& list) {
	ar.size_tag(list.size());

	for (auto const& i : list)
		ar(i);
}

//! Loading for std::list
template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::list<T, A>& list) {
	const auto size = ar.size_tag();
	const auto reservable = ar.template safe_to_reserve<T>(size);

	if (reservable == size) {
		list.resize(reservable);
		for (auto& i : list)
			ar(i);
	} else {
		for (typename Archive::size_type i = 0; i < size; ++i) {
			T& element = list.emplace_back();
			ar(element);
		}
	}
}

} // namespace vide --------------------------------------------------------------------------------
