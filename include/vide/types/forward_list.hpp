#pragma once

#include <forward_list>

#include <vide/macros.hpp>
#include <vide/size_tag.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for std::forward_list all other types
template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::forward_list<T, A> const& forward_list) {
	// write the size - note that this is slow because we need to traverse
	// the entire list. there are ways we could avoid this but this was chosen
	// since it works in the most general fashion with any archive type
	size_type const size = std::distance(forward_list.begin(), forward_list.end());

	ar.size_tag(size);

	// write the list
	for (const auto& i : forward_list)
		ar(i);
}

//! Loading for std::forward_list all other types from
template <class Archive, class T, class A>
void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::forward_list<T, A>& forward_list) {
	size_type size;
	ar.size_tag(size);

	forward_list.resize(static_cast<size_t>(size));

	for (auto& i : forward_list)
		ar(i);
}

} // namespace vide --------------------------------------------------------------------------------
