#pragma once

#include <deque>

#include <vide/macros.hpp>
#include <vide/size_tag.hpp>


namespace vide { // --------------------------------------------------------------------------------

template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::deque<T, A> const& deque) {
	ar.size_tag(static_cast<size_type>(deque.size()));

	for (auto const& i : deque)
		ar(i);
}

template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::deque<T, A>& deque) {
	size_type size;
	ar.size_tag(size);

	deque.resize(static_cast<size_t>(size));

	for (auto& i : deque)
		ar(i);
}

} // namespace vide --------------------------------------------------------------------------------

