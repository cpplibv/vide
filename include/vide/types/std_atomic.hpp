#pragma once

#include <vide/serializer.hpp>

#include <atomic>
#include <cstdint>


namespace vide { // --------------------------------------------------------------------------------

template <class T>
struct serializer<std::atomic<T>> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, std::atomic<T>& var, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar(var.load());
		} else {
			T value;
			ar(value);
			var.store(value);
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
