#pragma once

#include <vide/serializer.hpp>

#include <cstdint>
#include <string>


namespace vide { // --------------------------------------------------------------------------------

template <class CharT, class Traits, class Alloc>
struct serializer<std::basic_string<CharT, Traits, Alloc>> {
	template <class Archive>
	static void serialize(Archive& ar, std::basic_string<CharT, Traits, Alloc>& str, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.size_tag(str.size());
			if constexpr (Archive::template supports_binary<CharT>)
				ar.binary_data(str.data(), str.size());
			else
				for (const CharT& element : str)
					ar(element);
		} else {
			const auto size = ar.size_tag();

			if constexpr (Archive::template supports_binary<CharT>) {
				ar.template validate_read_size<CharT>(size);
				str.resize(size);
				ar.binary_data(str.data(), str.size());

			} else {
				const auto reservable = ar.template safe_to_reserve<CharT>(size);
				if (reservable == size) {
					str.resize(reservable);
					for (CharT& element : str)
						ar(element);

				} else {
					str.clear();
					str.reserve(reservable);
					for (typename Archive::size_type i = 0; i < size; ++i) {
						CharT& element = str.emplace_back();
						ar(element);
					}
				}
			}
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
