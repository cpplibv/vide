#pragma once

#include <vide/serializer.hpp>

#include <cstdint>
#include <optional>


namespace vide { // --------------------------------------------------------------------------------

template <typename T>
struct serializer<std::optional<T>> {
	template <class Archive>
	static void serialize(Archive& ar, std::optional<T>& optional, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.nvp("has_value", optional.has_value());
			if (optional)
				ar.nvp("value", *optional);
		} else {
			bool has_value;
			ar.nvp("has_value", has_value);

			if (has_value) {
				optional.emplace();
				ar.nvp("value", *optional);
			} else {
				optional = std::nullopt;
			}
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
