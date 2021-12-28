/*! \file variant.hpp
    \brief Support for std::variant
    \ingroup STLSupport */
/*
  Copyright (c) 2014, 2017, Randolph Voorhies, Shane Grant, Juan Pedro
  Bolivar Puente. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TYPES_STD_VARIANT_HPP_
#define CEREAL_TYPES_STD_VARIANT_HPP_

#include <variant>
#include <cstdint>

#include <cereal/macros.hpp>
#include <cereal/nvp.hpp>
#include <cereal/size_tag.hpp>


namespace cereal {
namespace detail {

// -------------------------------------------------------------------------------------------------

template <std::size_t N, typename Archive, typename... Types>
inline void load_std_variant(std::size_t index, Archive& ar, std::variant<Types...>& variant) {
	if (N == index) {
		variant.template emplace<N>();
		ar & CEREAL_NVP_("value", std::get<N>(variant));
	} else if constexpr (N + 1 < sizeof...(Types))
		load_std_variant<N + 1>(index, ar, variant);
}

} // namespace detail ------------------------------------------------------------------------------

template <typename Archive, typename... Types>
inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const std::variant<Types...>& variant) {
	static_assert(sizeof...(Types) < 256, "Variant serialization only supported up to 255 type.");

	const auto index = static_cast<uint8_t>(variant.index());
	ar & CEREAL_NVP_("type", index);
	std::visit([&](const auto& value) {
		ar & CEREAL_NVP(value);
	}, variant);
}

template <typename Archive, typename... Types>
inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, std::variant<Types...>& variant) {
	static_assert(sizeof...(Types) < 256, "Variant serialization only supported up to 255 type.");

	uint8_t index;
	ar & CEREAL_NVP_("type", index);
	if (index >= sizeof...(Types))
		throw cereal::Exception("Invalid 'index' when deserializing std::variant");

	detail::load_std_variant<0>(index, ar, variant);
}

/// Serializing a std::monostate
template <class Archive>
void CEREAL_SERIALIZE_FUNCTION_NAME(Archive&, const std::monostate&) {}

#endif // CEREAL_TYPES_STD_VARIANT_HPP_

} // namespace cereal
