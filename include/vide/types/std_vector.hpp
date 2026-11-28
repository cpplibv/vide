#pragma once

#include <vide/serializer.hpp>

#include <cstdint>
#include <vector>


namespace vide { // --------------------------------------------------------------------------------

template <typename T, typename A>
struct serializer<std::vector<T, A>> {
	template <typename Archive>
	static void serialize(Archive& ar, std::vector<T, A>& vector, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.size_tag(vector.size());
			if constexpr (Archive::template supports_binary<T>)
				ar.binary_data(vector.data(), vector.size());
			else
				for (const auto& v : vector)
					ar(v);
		} else {
			const auto size = ar.size_tag();
			if constexpr (Archive::template supports_binary<T>) {
				ar.template validate_read_size<T>(size);
				vector.resize(size);
				ar.binary_data(vector.data(), vector.size());

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
	}
};

template <typename A>
struct serializer<std::vector<bool, A>> {
	template <typename Archive>
	static void serialize(Archive& ar, std::vector<bool, A>& vector, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.size_tag(vector.size());
			for (const auto v : vector)
				ar(static_cast<bool>(v));
		} else {
			const auto size = ar.size_tag();
			// Packed storage: 8 elements per byte of budget
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
	}
};

} // namespace vide --------------------------------------------------------------------------------
