#pragma once

#include <vide/exception.hpp>
#include <vide/serializer.hpp>

#include <variant>
#include <cstdint>


namespace vide { // --------------------------------------------------------------------------------
namespace detail {

template <std::size_t N, typename Archive, typename... Types>
inline void load_std_variant(std::size_t index, Archive& ar, std::variant<Types...>& variant) {
	if (N == index) {
		variant.template emplace<N>();
		ar.nvp("value", std::get<N>(variant));
	} else if constexpr (N + 1 < sizeof...(Types))
		load_std_variant<N + 1>(index, ar, variant);
}

} // namespace detail ------------------------------------------------------------------------------

template <>
struct serializer<std::monostate> {
	template <class Archive>
	static void serialize(Archive&, std::monostate&, std::uint32_t) {}
};

template <typename... Types>
struct serializer<std::variant<Types...>> {
	static_assert(sizeof...(Types) < 256, "Variant serialization only supported up to 255 type.");

	template <class Archive>
	static void serialize(Archive& ar, std::variant<Types...>& variant, std::uint32_t) {
		if constexpr (Archive::is_output) {
			const auto index = static_cast<uint8_t>(variant.index());
			ar.nvp("type", index);
			std::visit([&](const auto& value) {
				ar.nvp("value", value);
			}, variant);
		} else {
			uint8_t index;
			ar.nvp("type", index);
			if (index >= sizeof...(Types))
				throw vide::Exception("Invalid 'index' when deserializing std::variant");

			detail::load_std_variant<0>(index, ar, variant);
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
