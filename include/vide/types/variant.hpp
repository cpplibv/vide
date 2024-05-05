#pragma once

#include <variant>
#include <cstdint>

#include <vide/macros.hpp>


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

/// Serializing a std::monostate
template <class Archive>
void VIDE_FUNCTION_NAME_SERIALIZE(Archive&, const std::monostate&) {
}

template <typename Archive, typename... Types>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::variant<Types...>& variant) {
	static_assert(sizeof...(Types) < 256, "Variant serialization only supported up to 255 type.");

	const auto index = static_cast<uint8_t>(variant.index());
	ar.nvp("type", index);
	std::visit([&](const auto& value) {
		ar.nvp("value", value);
	}, variant);
}

template <typename Archive, typename... Types>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::variant<Types...>& variant) {
	static_assert(sizeof...(Types) < 256, "Variant serialization only supported up to 255 type.");

	uint8_t index;
	ar.nvp("type", index);
	if (index >= sizeof...(Types))
		throw vide::Exception("Invalid 'index' when deserializing std::variant");

	detail::load_std_variant<0>(index, ar, variant);
}

} // namespace vide --------------------------------------------------------------------------------
