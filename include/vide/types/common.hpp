#pragma once

#include <vide/vide.hpp>


namespace vide {

// -------------------------------------------------------------------------------------------------

namespace common_detail {

//! Serialization for arrays if BinaryData is supported and we are arithmetic
/*! @internal */
template <class Archive, class T> inline
void serializeArray(Archive& ar, T& array, std::true_type /* binary_supported */) {
	ar(binary_data(array, sizeof(array)));
}

//! Serialization for arrays if BinaryData is not supported or we are not arithmetic
/*! @internal */
template <class Archive, class T> inline
void serializeArray(Archive& ar, T& array, std::false_type /* binary_supported */) {
	for (auto& i : array)
		ar(i);
}

namespace { // -------------------------------------------------------------------------------------

//! Gets the underlying type of an enum
/*! @internal */
template <class T, bool IsEnum>
struct enum_underlying_type : std::false_type {};

//! Gets the underlying type of an enum
/*! Specialization for when we actually have an enum
	@internal */
template <class T>
struct enum_underlying_type<T, true> { using type = typename std::underlying_type_t<T>; };

} // namespace -------------------------------------------------------------------------------------


//! Checks if a type is an enum
/*! This is needed over simply calling std::is_enum because the type
	traits checking at compile time will attempt to call something like
	load_minimal with a special NoConvertRef struct that wraps up the true type.

	This will strip away any of that and also expose the true underlying type.
	@internal */
template <class T>
class is_enum {
private:
	using DecayedT = typename std::decay_t<T>;
	using StrippedT = typename ::vide::traits::strip_minimal<DecayedT>::type;

public:
	static const bool value = std::is_enum_v<StrippedT>;
	using type = StrippedT;
	using base_type = typename enum_underlying_type<StrippedT, value>::type;
};

} // namespace common_detail -----------------------------------------------------------------------

//! Saving for enum types
template <class Archive, class T> inline
typename std::enable_if<common_detail::is_enum<T>::value,
		typename common_detail::is_enum<T>::base_type>::type
VIDE_FUNCTION_NAME_SAVE_MINIMAL(const Archive&, T const& t) {
	return static_cast<typename common_detail::is_enum<T>::base_type>(t);
}

//! Loading for enum types
template <class Archive, class T> inline
typename std::enable_if<common_detail::is_enum<T>::value, void>::type
VIDE_FUNCTION_NAME_LOAD_MINIMAL(const Archive&, T&& t,
		typename common_detail::is_enum<T>::base_type const& value) {
	t = reinterpret_cast<typename common_detail::is_enum<T>::type const&>( value );
}

//! Serialization for raw pointers
/*! This exists only to throw a static_assert to let users know we don't support raw pointers. */
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_SERIALIZE(Archive&, T*&) {
	static_assert(vide::traits::detail::delay_static_assert<T>::value,
			"Cereal does not support serializing raw pointers - please use a smart pointer");
}

//! Serialization for C style arrays
template <class Archive, class T>
inline typename std::enable_if<std::is_array_v<T>, void>::type
VIDE_FUNCTION_NAME_SERIALIZE(Archive& ar, T& array) {
	common_detail::serializeArray(ar, array,
			std::integral_constant<bool, traits::is_output_serializable<BinaryData<T>, Archive>::value &&
					std::is_arithmetic_v<typename std::remove_all_extents<T>::type>>());
}

// -------------------------------------------------------------------------------------------------

} // namespace vide
