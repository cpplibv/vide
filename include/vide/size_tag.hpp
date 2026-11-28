#pragma once

#include <vide/exception.hpp>

#include <cstdint>
#include <limits>
#include <string>


namespace vide { // --------------------------------------------------------------------------------

/// Tag of a container size, created by ar.size_tag(). Binary archives store the size; JSON and XML store
/// none and count the children of the node on load, so a hand edited array stays loadable.
/// Size is size_type when saving, size_type& when loading
template <class Size>
struct SizeTag {
	Size size;
};

namespace detail {

/// Throws when size does not fit Size: a truncated size would silently corrupt the data
template <class Size>
[[nodiscard]] constexpr Size checked_size(std::uint64_t size) {
	if constexpr (std::numeric_limits<Size>::max() < std::numeric_limits<std::uint64_t>::max())
		if (size > std::numeric_limits<Size>::max())
			throw Exception("Container size " + std::to_string(size) + " exceeds the maximum of the archive size type " +
					std::to_string(std::numeric_limits<Size>::max()) + ".");

	return static_cast<Size>(size);
}

} // namespace detail

} // namespace vide --------------------------------------------------------------------------------
