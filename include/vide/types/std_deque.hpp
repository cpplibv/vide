#pragma once

#include <vide/serializer.hpp>

#include <cstdint>
#include <deque>


namespace vide { // --------------------------------------------------------------------------------

template <class T, class A>
struct serializer<std::deque<T, A>> {
	template <class Archive>
	static void serialize(Archive& ar, std::deque<T, A>& deque, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.size_tag(deque.size());

			for (auto const& i : deque)
				ar(i);
		} else {
			const auto size = ar.size_tag();
			const auto reservable = ar.template safe_to_reserve<T>(size);

			if (reservable == size) {
				deque.resize(reservable);
				for (auto& i : deque)
					ar(i);
			} else {
				deque.clear();
				// deque.reserve(reservable); // No reserve for std::deque
				for (typename Archive::size_type i = 0; i < size; ++i) {
					T& element = deque.emplace_back();
					ar(element);
				}
			}
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------

