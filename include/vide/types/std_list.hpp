#pragma once

#include <vide/serializer.hpp>

#include <cstdint>
#include <list>


namespace vide { // --------------------------------------------------------------------------------

template <class T, class A>
struct serializer<std::list<T, A>> {
	template <class Archive>
	static void serialize(Archive& ar, std::list<T, A>& list, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.size_tag(list.size());

			for (auto const& i : list)
				ar(i);
		} else {
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
	}
};

} // namespace vide --------------------------------------------------------------------------------
