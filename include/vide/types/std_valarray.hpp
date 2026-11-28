#pragma once

#include <vide/serializer.hpp>

#include <cstdint>
#include <valarray>
#include <vector>


namespace vide { // --------------------------------------------------------------------------------

template <class T>
struct serializer<std::valarray<T>> {
	template <class Archive>
	static void serialize(Archive& ar, std::valarray<T>& valarray, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.size_tag(valarray.size()); // number of elements

			if constexpr (Archive::template supports_binary<T>)
				ar.binary_data(std::begin(valarray), valarray.size());
			else
				for (const auto& i : valarray)
					ar(i);
		} else {
			const auto size = ar.size_tag();

			if constexpr (Archive::template supports_binary<T>) {
				ar.template validate_read_size<T>(size);
				valarray.resize(size);
				ar.binary_data(std::begin(valarray), valarray.size());

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
	}
};

} // namespace vide --------------------------------------------------------------------------------
