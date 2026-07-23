#pragma once

#include <vide/macros.hpp>

#include <meta>


namespace vide { // --------------------------------------------------------------------------------

namespace detail {

template <std::size_t N>
consteval std::array<int, N> make_index_array() {
	std::array<int, N> indices;
	for (std::size_t i = 0; i < N; ++i)
		indices[i] = static_cast<int>(i);
	return indices;
}

// consteval auto to_array(std::span<std::meta::info> span) {
// 	static constexpr std::size_t N = span.size();
// 	std::array<std::meta::info, N> result;
// 	for (std::size_t i = 0; i < N; ++i)
// 		result[i] = span[i];
// 	return result;
// }

} // namespace detail

// -------------------------------------------------------------------------------------------------

template <class Archive, class T>
	requires requires { typename T::serialize_enable_reflection; }
inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive& ar, T& var) {
	static constexpr auto members = define_static_array(nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()));
	template for (constexpr std::meta::info member : members) {
		static constexpr auto annotations = define_static_array(annotations_of(member));
		static constexpr auto validators = define_static_array([] {
			std::vector<std::meta::info> result;
			template for (constexpr std::meta::info annotation : annotations) {
				if constexpr (requires (const T& v) { { [:constant_of(annotation):](v) }; } ) {
					result.emplace_back(annotation);
				}
			}
			return result;
		}());
		[[maybe_unused]] // maybe_unused as GCC 16.1 has a false positive warning for unused pack with splice
		static constexpr auto [...validatorsIndices] = detail::make_index_array<validators.size()>();

		ar.nvp(identifier_of(member).data(), var.[:member:], [:constant_of(validators[validatorsIndices]):]...);
		// ar.nvp(identifier_of(member).data(), var.[:member:], [:constant_of(validators):]...);
	}
}

} // namespace vide --------------------------------------------------------------------------------

