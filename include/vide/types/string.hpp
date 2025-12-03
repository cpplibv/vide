#pragma once

#include <string>

#include <vide/macros.hpp>
#include <vide/binary_data.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Serialization for basic_string types, if binary data is supported
template <class Archive, class CharT, class Traits, class Alloc>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::basic_string<CharT, Traits, Alloc>& str) {
	ar.size_tag(str.size());
	ar(binary_data(str.data(), str.size() * sizeof(CharT)));
}

//! Serialization for basic_string types, if binary data is supported
template <class Archive, class CharT, class Traits, class Alloc>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::basic_string<CharT, Traits, Alloc>& str) {
	const auto size = ar.size_tag();

	if constexpr (Archive::template supports_binary<CharT>) {
		ar.template validate_read_size<CharT>(size);
		str.resize(size);
		ar(binary_data(str.data(), str.size() * sizeof(CharT)));

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

} // namespace vide --------------------------------------------------------------------------------
