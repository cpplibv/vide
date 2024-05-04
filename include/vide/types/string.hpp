#pragma once

#include <string>

#include <vide/macros.hpp>
#include <vide/nvp.hpp>
#include <vide/size_tag.hpp>
#include <vide/binary_data.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Serialization for basic_string types, if binary data is supported
template <class Archive, class CharT, class Traits, class Alloc>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::basic_string<CharT, Traits, Alloc> const& str) {
	// Save number of chars + the data
	ar.size_tag(static_cast<size_type>(str.size()));
	ar(binary_data(str.data(), str.size() * sizeof(CharT)));
}

//! Serialization for basic_string types, if binary data is supported
template <class Archive, class CharT, class Traits, class Alloc>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::basic_string<CharT, Traits, Alloc>& str) {
	size_type size;
	ar.size_tag(size);
	str.resize(static_cast<std::size_t>(size));
	ar(binary_data(const_cast<CharT*>(str.data()), static_cast<std::size_t>(size) * sizeof(CharT)));
}

} // namespace vide --------------------------------------------------------------------------------
