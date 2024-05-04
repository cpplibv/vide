#pragma once

#include <list>

#include <vide/macros.hpp>
#include <vide/size_tag.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for std::list
template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::list<T, A> const& list) {
	ar.size_tag(static_cast<size_type>(list.size()));

	for (auto const& i : list)
		ar(i);
}

//! Loading for std::list
template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::list<T, A>& list) {
	size_type size;
	ar.size_tag(size);

	list.resize(static_cast<size_t>(size));

	for (auto& i : list)
		ar(i);
}

} // namespace vide --------------------------------------------------------------------------------
