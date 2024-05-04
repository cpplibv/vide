#pragma once

#include <type_traits>
#include <typeindex>

#include <vide/access.hpp>
#include <vide/concept.hpp>
#include <vide/macros.hpp>


namespace vide {
namespace traits {

// -------------------------------------------------------------------------------------------------

using yes = std::true_type;
using no = std::false_type;

namespace detail {

//! Used to delay a static_assert until template instantiation
template <class T>
struct delay_static_assert : std::false_type {};

// SFINAE Helpers

//! Return type for SFINAE Enablers
enum class sfinae {};

// Helper functionality for boolean integral constants and Enable/DisableIf
template <bool H, bool ... T> struct meta_bool_and : std::integral_constant<bool, H && meta_bool_and<T...>::value> {};
template <bool B> struct meta_bool_and<B> : std::integral_constant<bool, B> {};

// workaround needed due to bug in MSVC 2013, see
// http://connect.microsoft.com/VisualStudio/feedback/details/800231/c-11-alias-template-issue
template <bool ... Conditions>
struct EnableIfHelper : std::enable_if<meta_bool_and<Conditions...>::value, sfinae> {};

} // namespace detail

//! Used as the default value for EnableIf and DisableIf template parameters
/*! @relates EnableIf
	@relates DisableIf */
static const detail::sfinae sfinae = {};

// ######################################################################
//! Provides a way to enable a function if conditions are met
/*! This is intended to be used in a near identical fashion to std::enable_if
	while being significantly easier to read at the cost of not allowing for as
	complicated of a condition.

	This will compile (allow the function) if every condition evaluates to true.
	at compile time.  This should be used with SFINAE to ensure that at least
	one other candidate function works when one fails due to an EnableIf.

	This should be used as the las template parameter to a function as
	an unnamed parameter with a default value of vide::traits::sfinae:

	@code{cpp}
	// using by making the last template argument variadic
	template <class T, EnableIf<std::is_same<T, bool>::value> = sfinae>
	void func(T t );
	@endcode

	Note that this performs a logical AND of all conditions, so you will need
	to construct more complicated requirements with this fact in mind.

	@relates DisableIf
	@relates sfinae
	@tparam Conditions The conditions which will be logically ANDed to enable the function. */
template <bool ... Conditions>
using EnableIf = typename detail::EnableIfHelper<Conditions...>::type;

// -------------------------------------------------------------------------------------------------

// NOTE: The get_output_from_input only used for save_minimal/load_minimal
//			and there only to determine the return type of save_minimal
//
//template <typename Archive>
//using get_output_from_input = typename Archive::ArchiveOutput;

// -------------------------------------------------------------------------------------------------

//! Determines whether the class T can be default constructed by vide::access
template <class T>
concept is_default_constructible = requires { vide::access::construct<T>(); };

} // namespace traits ------------------------------------------------------------------------------

// =================================================================================================
// =================================================================================================
// =================================================================================================

// TODO P2: Implement diagnose_type
// TODO P3: Split has_ functions into has_ and diagnose_. has only asks one questions and has no static assert, while diagnose_ tries to check everything. diagnose_ only gets called if a serialize funtion is not found.
// template<class T, class A>
// struct has_member_load_minimal_versioned { ...
// template<class T, class A>
// struct diagnose_member_load_minimal_versioned { ...

// // Load Minimal Utilities
// namespace detail {
//
// //! Used to help strip away conversion wrappers
// /*! If someone writes a non-member load/save minimal function that accepts its
// 	parameter as some generic template type and needs to perform trait checks
// 	on that type, our NoConvert wrappers will interfere with this.  Using
// 	the struct strip_minmal, users can strip away our wrappers to get to
// 	the underlying type, allowing traits to work properly */
// struct NoConvertBase {};
//
// //! A struct that prevents implicit conversion
// /*! Any type instantiated with this struct will be unable to implicitly convert
// 	to another type.  Is designed to only allow conversion to Source &.
//
// 	@tparam Source the type of the original source */
// template <class Source>
// struct NoConvertRef : NoConvertBase {
// 	using type = Source; //!< Used to get underlying type easily
//
// 	template <class Dest, class = typename std::enable_if<std::is_same<Source, Dest>::value>::type>
// 	operator Dest() = delete;
//
// #ifdef __clang__
// 	template <class Dest, class = typename std::enable_if<std::is_same<Source, Dest>::value>::type>
// 	operator Dest const & () = delete;
// #endif // __clang__
//
// 	//! only allow conversion if the types are the same and we are converting into a const reference
// 	template <class Dest, class = typename std::enable_if<std::is_same<Source, Dest>::value>::type>
// 	operator Dest&();
// };
//
// //! A struct that prevents implicit conversion to a different type
// template <class Source>
// 	requires (!std::is_void_v<Source>)
// struct NoConvertAnyRef : NoConvertBase {
// 	using type = Source; //!< Used to get underlying type easily
//
// 	operator Source&();
// 	operator Source&&();
// 	operator const Source&() const;
// };
//
// //! A type that can implicitly convert to anything else
// struct AnyConvert {
// 	template <class Dest>
// 	operator Dest&();
//
// 	template <class Dest>
// 	operator Dest&&();
//
// 	template <class Dest>
// 	operator const Dest&() const;
// };
//
// } // namespace detail ------------------------------------------------------------------------------

template <typename Archive, typename T>
struct diagnose_type {
	// When a request for serialization fails, instantiate this type and
	// ALL THE DEBUG INFORMATION HERE

	// template <class T, class A>
	// struct has_member_save : std::integral_constant<bool, detail::has_member_save_impl<T, A>::value> {
	// 	typedef typename detail::has_member_save_impl<T, A> check;
	// 	static_assert(check::value || !check::not_const_type,
	// 			"Vide detected a non-const save. \n "
	// 			"save member functions must always be const");
	// };
	//
	// // ######################################################################
	// // Member Save (versioned)
	// VIDE_MAKE_HAS_MEMBER_SAVE_IMPL(versioned_save, VIDE_MAKE_VERSIONED_TEST)
	//
	// template <class T, class A>
	// struct has_member_versioned_save : std::integral_constant<bool, detail::has_member_versioned_save_impl<T, A>::value> {
	// 	typedef typename detail::has_member_versioned_save_impl<T, A> check;
	// 	static_assert(check::value || !check::not_const_type,
	// 			"Vide detected a versioned non-const save. \n "
	// 			"save member functions must always be const");
	// };
	//
	// template <class T, class A>                                                                                              \
	// struct has_global_##test_name : std::integral_constant<bool, detail::has_global_##test_name##_impl<T, A>::value> \
	// {                                                                                                                        \
	// 	using check = typename detail::has_global_##test_name##_impl<T, A>;                                                \
	// 	static_assert( check::value || !check::not_const_type,                                                                 \
	// 	"Vide detected a non-const type parameter in non-member " #test_name ". \n "                                       \
	// 	#test_name " non-member functions must always pass their types as const" );                                          \
	// };
	//
	// template <class T, class A>                                                                                      \
	// struct has_member_##test_name : std::integral_constant<bool, detail::has_member_##test_name##_impl<T, A>::value> \
	// {                                                                                                                \
	// 	using check = typename detail::has_member_##test_name##_impl<T, A>;                                            \
	// 	static_assert( check::valid,                                                                                   \
	// 	"Vide detected a non-const member " #test_name ". \n "                                                       \
	// 	#test_name " member functions must always be const" );                                                       \
	// 	\
	// 	using type = typename detail::get_member_##test_name##_type<T, A, check::value>::type;                         \
	// };
	//
	// template <class T, class A>                                                                                              \
	// struct has_global_##test_name : std::integral_constant<bool, detail::has_global_##test_name##_impl<T, A>::value> \
	// {                                                                                                                        \
	// 	using check = typename detail::has_global_##test_name##_impl<T, A>;                                                \
	// 	static_assert( check::valid,                                                                                           \
	// 	"Vide detected a non-const type parameter in non-member " #test_name ". \n "                                         \
	// 	#test_name " non-member functions must always pass their types as const" );                                          \
	// 	\
	// 	using type = typename detail::get_non_member_##test_name##_type<T, A, check::value>::type;                             \
	// };
	//
	//
	// template<class T, class A>
	// struct has_member_load_minimal {
	// 	using save_type = get_member_save_minimal_t<A, T>;
	//
	// 	// Strategy: Check if a function matching the signature more or less exists
	// 	// (allow anything like load_minimal(xxx) using AnyConvert, and then enforce that it has the correct signature using NoConvertAnyRef
	// 	static constexpr bool exists = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert()); };
	// 	static constexpr bool value = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>()); };
	//
	// 	static_assert(exists == value, "Vide detected different or invalid types in corresponding member " "load_minimal" " and " "save_minimal" " functions. \n " "the paramater to " "load_minimal" " must be the type (optionally: value, &, const& or &&) that " "save_minimal" " returns.");
	// };
	//
	// template<class T, class A>
	// struct has_member_load_minimal_versioned {
	// 	using save_type = get_member_save_minimal_versioned_t<A, T>;
	//
	// 	// Strategy: Check if a function matching the signature more or less exists
	// 	// (allow anything like load_minimal(xxx) using AnyConvert, and then enforce that it has the correct signature using NoConvertAnyRef
	// 	static constexpr bool exists = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert(), detail::AnyConvert()); };
	// 	static constexpr bool value = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>(), std::declval<uint32_t>()); };
	//
	// 	static_assert(exists == value, "Vide detected different or invalid types in corresponding member " "load_minimal_versioned" " and " "save_minimal_versioned" " functions. \n " "the paramater to " "load_minimal_versioned" " must be the type (optionally: value, &, const& or &&) that " "save_minimal_versioned" " returns.");
	// };
	//
	// template<class T, class A>
	// struct has_global_load_minimal {
	// 	using save_type = get_global_save_minimal_t<A, T>;
	//
	// 	// Note that there should be an additional const check on load_minimal after the valid check,
	// 	// but this currently interferes with many valid uses of minimal serialization.  It has been
	// 	// removed (see #565 on github) and previously was:
	// 	//
	// 	// @code
	// 	// static_assert( check::const_valid || !check::exists,
	// 	// 	"Vide detected an invalid serialization type parameter in non-member " #test_name ".  "
	// 	// 	#test_name " non-member functions must accept their serialization type by non-const reference" );
	// 	// @endcode
	//
	// 	static constexpr bool exists = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert()); };
	// 	static constexpr bool valid = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>()); };
	// 	static constexpr bool const_valid = requires { load_minimal(std::declval<const A&>(), detail::NoConvertRef<T>(), detail::AnyConvert()); };
	//
	// 	static_assert(!exists || (valid && const_valid), "Vide detected different types in corresponding non-member " "load_minimal" " and " "save_minimal" " functions. \n " "the paramater to " "load_minimal" " must be a constant reference to the type that " "save_minimal" " returns.");
	// 	static constexpr bool value = exists && valid && const_valid;
	// };
	//
	// template<class T, class A>
	// struct has_global_load_minimal_versioned {
	// 	using save_type = get_global_save_minimal_versioned_t<A, T>;
	//
	// 	static constexpr bool exists = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert(), std::declval<uint32_t>()); };
	// 	static constexpr bool valid = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>(), std::declval<uint32_t>()); };
	// 	static constexpr bool const_valid = requires { load_minimal(std::declval<const A&>(), detail::NoConvertRef<T>(), detail::AnyConvert(), std::declval<uint32_t>()); };
	//
	// 	static_assert(!exists || (valid && const_valid), "Vide detected different types in corresponding non-member " "load_minimal_versioned" " and " "save_minimal_versioned" " functions. \n " "the paramater to " "load_minimal_versioned" " must be a constant reference to the type that " "save_minimal_versioned" " returns.");
	// 	static constexpr bool value = exists && valid && const_valid;
	// };

	// // ######################################################################
	// template <class T, class OutputArchive>
	// struct has_invalid_output_versioning : std::integral_constant<bool,
	// 		// (has_member_versioned_save<T, OutputArchive>::value && has_member_save<T, OutputArchive>::value) ||
	// 		(has_global_versioned_save<T, OutputArchive>::value && has_global_save<T, OutputArchive>::value) ||
	// 		(has_member_versioned_serialize<T, OutputArchive>::value && has_member_serialize<T, OutputArchive>::value) ||
	// 		(has_global_versioned_serialize<T, OutputArchive>::value && has_global_serialize<T, OutputArchive>::value) ||
	// 		// (has_member_save_minimal_versioned<T, OutputArchive>::value && has_member_save_minimal<T, OutputArchive>::value) ||
	// 		(has_global_save_minimal_versioned<T, OutputArchive>::value && has_global_save_minimal<T, OutputArchive>::value)> {
	// };
	//
	// // ######################################################################
	// template <class T, class InputArchive>
	// struct has_invalid_input_versioning : std::integral_constant<bool,
	// 		(has_member_versioned_load<T, InputArchive>::value && has_member_load<T, InputArchive>::value) ||
	// 		(has_global_versioned_load<T, InputArchive>::value && has_global_load<T, InputArchive>::value) ||
	// 		(has_member_versioned_serialize<T, InputArchive>::value && has_member_serialize<T, InputArchive>::value) ||
	// 		(has_global_versioned_serialize<T, InputArchive>::value && has_global_serialize<T, InputArchive>::value) ||
	// 		(has_member_load_minimal_versioned<T, InputArchive>::value && has_member_load_minimal<T, InputArchive>::value) ||
	// 		(has_global_load_minimal_versioned<T, InputArchive>::value && has_global_load_minimal<T, InputArchive>::value)> {
	// };
};

// -------------------------------------------------------------------------------------------------

} // namespace vide
