#pragma once

#include <vide/macros.hpp>
#include <vide/serializer.hpp>

#include <cstdint>
#include <forward_list>
#include <ranges>


namespace vide { // --------------------------------------------------------------------------------

template <class T, class A>
struct serializer<std::forward_list<T, A>> {
	template <class Archive>
	static void serialize(Archive& ar, std::forward_list<T, A>& forward_list, std::uint32_t) {
		if constexpr (Archive::is_output) {
			// write the size - note that this is slow because we need to traverse
			// the entire list. there are ways we could avoid this but this was chosen
			// since it works in the most general fashion with any archive type
			ar.size_tag(static_cast<VIDE_SIZE_TYPE>(std::distance(forward_list.begin(), forward_list.end())));

			// write the list
			for (const auto& i : forward_list)
				ar(i);
		} else {
			const auto size = ar.size_tag();
			const auto reservable = ar.template safe_to_reserve<T>(size);

			if (reservable == size) {
				forward_list.resize(reservable);
				for (auto& i : forward_list)
					ar(i);
			} else {
				auto gen = std::views::iota(0u, size) | std::views::transform([&](auto) {
					T value;
					ar(value);
					return value;
				});
				forward_list.assign(gen.begin(), gen.end());
			}
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
