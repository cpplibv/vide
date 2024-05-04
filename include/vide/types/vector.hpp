#pragma once

#include <vector>

#include <vide/binary_data.hpp>
#include <vide/macros.hpp>
#include <vide/size_tag.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Serialization for non-bool vector types
template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::vector<T, A>& vector) {
	ar.size_tag(static_cast<size_type>(vector.size())); // number of elements

	if constexpr (Archive::template supports_binary<T>) {
		ar(binary_data(vector.data(), vector.size() * sizeof(T)));
	} else {
		for (const auto& v : vector)
			ar(v);
	}
}

//! Serialization for non-bool vector types
template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::vector<T, A>& vector) {
	typename Archive::size_type size;
	ar.size_tag(size);
	vector.resize(static_cast<std::size_t>(size));

	if constexpr (Archive::template supports_binary<T>) {
		ar(binary_data(vector.data(), vector.size() * sizeof(T)));
	} else {
		for (auto& v : vector)
			ar(v);
	}
}

//! Serialization for bool vector types
template <class Archive, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::vector<bool, A>& vector) {
	ar.size_tag(static_cast<size_type>(vector.size())); // number of elements
	for (const auto v : vector)
		ar(static_cast<bool>(v));
}

//! Serialization for bool vector types
template <class Archive, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::vector<bool, A>& vector) {
	size_type size;
	ar.size_tag(size);

	vector.resize(static_cast<std::size_t>(size));
	for (auto&& v : vector) {
		bool b;
		ar(b);
		v = b;
	}
}

} // namespace vide --------------------------------------------------------------------------------
