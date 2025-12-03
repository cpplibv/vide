#pragma once

#include <vector>

#include <vide/binary_data.hpp>
#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------

/// Serialization for non-bool vector types
template <typename Archive, typename T, typename A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::vector<T, A>& vector) {
	ar.size_tag(vector.size()); // number of elements

	if constexpr (Archive::template supports_binary<T>)
		ar(binary_data(vector.data(), vector.size() * sizeof(T)));
	else
		for (const auto& v : vector)
			ar(v);
}

/// Serialization for non-bool vector types
template <typename Archive, typename T, typename A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::vector<T, A>& vector) {
	const auto size = ar.size_tag();

	if constexpr (Archive::template supports_binary<T>) {
		ar.template validate_read_size<T>(size);
		vector.resize(size);
		ar(binary_data(vector.data(), vector.size() * sizeof(T)));

	} else {
		const auto reservable = ar.template safe_to_reserve<T>(size);
		if (reservable == size) {
			vector.resize(reservable);
			for (T& element : vector)
				ar(element);

		} else {
			vector.clear();
			vector.reserve(reservable);
			for (typename Archive::size_type i = 0; i < size; ++i) {
				T& element = vector.emplace_back();
				ar(element);
			}
		}
	}
}

/// Serialization for bool vector types
template <typename Archive, typename A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::vector<bool, A>& vector) {
	ar.size_tag(vector.size()); // number of elements

	for (const auto v : vector)
		ar(static_cast<bool>(v));
}

/// Serialization for bool vector types
template <typename Archive, typename A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::vector<bool, A>& vector) {
	const auto size = ar.size_tag();

	const auto reservable = ar.template safe_to_reserve<bool>(size / 8);
	if (reservable == size) {
		vector.resize(reservable);
		for (auto&& element : vector) {
			bool value;
			ar(value);
			element = value;
		}
	} else {
		vector.clear();
		vector.reserve(reservable);
		for (typename Archive::size_type i = 0; i < size; ++i) {
			auto&& element = vector.emplace_back();
			bool value;
			ar(value);
			element = value;
		}
	}
}

} // namespace vide --------------------------------------------------------------------------------
