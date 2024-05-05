#pragma once

#include <deque>

#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::deque<T, A> const& deque) {
	ar.size_tag(deque.size());

	for (auto const& i : deque)
		ar(i);
}

template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::deque<T, A>& deque) {
	const auto size = ar.size_tag();
	const auto reserveable = ar.template safe_to_reserve<T>(size);

	if (reserveable == size) {
		deque.resize(reserveable);
		for (auto& i : deque)
			ar(i);
	} else {
		deque.clear();
		// deque.reserve(reserveable); // No reserve for std::deque
		for (typename Archive::size_type i = 0; i < size; ++i) {
			T& element = deque.emplace_back();
			ar(element);
		}
	}
}

} // namespace vide --------------------------------------------------------------------------------

