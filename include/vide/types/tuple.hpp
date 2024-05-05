#pragma once

#include <tuple>

#include <vide/macros.hpp>


namespace vide { // --------------------------------------------------------------------------------
namespace tuple_detail {

//! Creates a c string from a sequence of characters
/*! The c string created will always be prefixed by "tuple_element"
	Based on code from: http://stackoverflow/a/20973438/710791
	@internal */
template <char... Cs>
struct char_seq_to_c_str {
	static const int size = 14; // Size of array for the word: tuple_element
	typedef const char (&arr_type)[sizeof...(Cs) + size];
	static const char str[sizeof...(Cs) + size];
};

// the word tuple_element plus a number
//! @internal
template <char... Cs>
const char char_seq_to_c_str<Cs...>::str[sizeof...(Cs) + size] =
		{'t', 'u', 'p', 'l', 'e', '_', 'e', 'l', 'e', 'm', 'e', 'n', 't', Cs..., '\0'};

//! Converts a number into a sequence of characters
/*! @tparam Q The quotient of dividing the original number by 10
	@tparam R The remainder of dividing the original number by 10
	@tparam C The sequence built so far
	@internal */
template <size_t Q, size_t R, char ... C>
struct to_string_impl {
	using type = typename to_string_impl<Q / 10, Q % 10, static_cast<char>(R + std::size_t{'0'}), C...>::type;
};

//! Base case with no quotient
/*! @internal */
template <size_t R, char ... C>
struct to_string_impl<0, R, C...> {
	using type = char_seq_to_c_str<static_cast<char>(R + std::size_t{'0'}), C...>;
};

//! Generates a c string for a given index of a tuple
/*! Example use:
	@code{cpp}
	tuple_element_name<3>::c_str();// returns "tuple_element3"
	@endcode
	@internal */
template <size_t T>
struct tuple_element_name {
	using type = typename to_string_impl<T / 10, T % 10>::type;

	static const typename type::arr_type c_str() { return type::str; }
};

// unwinds a tuple to save it
//! @internal
template <size_t Height>
struct serialize {
	template <class Archive, class... Types>
	inline static void apply(Archive& ar, std::tuple<Types...>& tuple) {
		serialize<Height - 1>::template apply(ar, tuple);
		ar.nvp(tuple_element_name<Height - 1>::c_str(), std::get<Height - 1>(tuple));
	}
};

// Zero height specialization - nothing to do here
//! @internal
template <>
struct serialize<0> {
	template <class Archive, class... Types>
	inline static void apply(Archive&, std::tuple<Types...>&) {
	}
};

} // namespace tuple_detail ------------------------------------------------------------------------

//! Serializing for std::tuple
template <class Archive, class... Types>
inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive& ar, std::tuple<Types...>& tuple) {
	tuple_detail::serialize<std::tuple_size<std::tuple<Types...>>::value>::template apply(ar, tuple);
}

} // namespace vide --------------------------------------------------------------------------------
