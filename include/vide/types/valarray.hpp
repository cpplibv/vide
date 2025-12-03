#pragma once

#include <valarray>
#include <vector>

#include <vide/binary_data.hpp>
#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

//! Saving for std::valarray all other types
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::valarray<T>& valarray) {
	ar.size_tag(valarray.size()); // number of elements

	if constexpr (Archive::template supports_binary<T>)
		ar(binary_data(&valarray[0], valarray.size() * sizeof(T)));
	else
		for (const auto& i : valarray)
			ar(i);
}

//! Loading for std::valarray all other types
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::valarray<T>& valarray) {
	const auto size = ar.size_tag();

	if constexpr (Archive::template supports_binary<T>) {
		ar.template validate_read_size<T>(size);
		valarray.resize(size);
		ar(binary_data(&valarray[0], valarray.size() * sizeof(T)));

	} else {
		const auto reservable = ar.template safe_to_reserve<T>(size);
		if (reservable == size) {
			valarray.resize(reservable);
			for (T& element : valarray)
				ar(element);

		} else {
			// std::valarray doesnt play nicely with modifications so fallback to vector emplace_back
			std::vector<T> temp;
			temp.reserve(reservable);
			for (typename Archive::size_type i = 0; i < size; ++i) {
				T& element = temp.emplace_back();
				ar(element);
			}

			// Resize is now safe as we successfully extracted this from the archive
			valarray.resize(size);
			for (typename Archive::size_type i = 0; i < size; ++i)
				valarray[i] = std::move(temp[i]);
		}
	}
}

} // namespace vide --------------------------------------------------------------------------------
