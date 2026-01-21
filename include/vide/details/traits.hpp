#pragma once

#include <vide/access.hpp>


namespace vide::traits { // ------------------------------------------------------------------------

//! Determines whether the class T can be default constructed by vide::access
template <class T>
concept is_default_constructible = requires { vide::access::construct<T>(); };

} // namespace vide::traits ------------------------------------------------------------------------

// =================================================================================================
// =================================================================================================
// =================================================================================================

// TODO P2: Implement diagnose_type
// TODO P3: Split has_ functions into has_ and diagnose_. has only asks one questions and has no static assert, while diagnose_ tries to check everything. diagnose_ only gets called if a serialize function is not found.
// template<class T, class A>
// struct has_member_load_minimal_versioned { ...
// template<class T, class A>
// struct diagnose_member_load_minimal_versioned { ...

// template <typename Archive, typename T>
// struct diagnose_type {
// 	// When a request for serialization fails, instantiate this type and
// 	// ALL THE DEBUG INFORMATION HERE
//
// 	template <class T, class A>
// 	struct has_member_save : std::integral_constant<bool, detail::has_member_save_impl<T, A>::value> {
// 		typedef typename detail::has_member_save_impl<T, A> check;
// 		static_assert(check::value || !check::not_const_type,
// 				"Vide detected a non-const save. \n "
// 				"save member functions must always be const");
// 	};
//
// 	// ######################################################################
// 	// Member Save (versioned)
// 	VIDE_MAKE_HAS_MEMBER_SAVE_IMPL(versioned_save, VIDE_MAKE_VERSIONED_TEST)
//
// 	template <class T, class A>
// 	struct has_member_versioned_save : std::integral_constant<bool, detail::has_member_versioned_save_impl<T, A>::value> {
// 		typedef typename detail::has_member_versioned_save_impl<T, A> check;
// 		static_assert(check::value || !check::not_const_type,
// 				"Vide detected a versioned non-const save. \n "
// 				"save member functions must always be const");
// 	};
//
// 	template <class T, class A>                                                                                              \
// 	struct has_global_##test_name : std::integral_constant<bool, detail::has_global_##test_name##_impl<T, A>::value> \
// 	{                                                                                                                        \
// 		using check = typename detail::has_global_##test_name##_impl<T, A>;                                                \
// 		static_assert( check::value || !check::not_const_type,                                                                 \
// 		"Vide detected a non-const type parameter in non-member " #test_name ". \n "                                       \
// 		#test_name " non-member functions must always pass their types as const" );                                          \
// 	};
//
// 	template <class T, class A>                                                                                      \
// 	struct has_member_##test_name : std::integral_constant<bool, detail::has_member_##test_name##_impl<T, A>::value> \
// 	{                                                                                                                \
// 		using check = typename detail::has_member_##test_name##_impl<T, A>;                                            \
// 		static_assert( check::valid,                                                                                   \
// 		"Vide detected a non-const member " #test_name ". \n "                                                       \
// 		#test_name " member functions must always be const" );                                                       \
// 		\
// 		using type = typename detail::get_member_##test_name##_type<T, A, check::value>::type;                         \
// 	};
//
// 	template <class T, class A>                                                                                              \
// 	struct has_global_##test_name : std::integral_constant<bool, detail::has_global_##test_name##_impl<T, A>::value> \
// 	{                                                                                                                        \
// 		using check = typename detail::has_global_##test_name##_impl<T, A>;                                                \
// 		static_assert( check::valid,                                                                                           \
// 		"Vide detected a non-const type parameter in non-member " #test_name ". \n "                                         \
// 		#test_name " non-member functions must always pass their types as const" );                                          \
// 		\
// 		using type = typename detail::get_non_member_##test_name##_type<T, A, check::value>::type;                             \
// 	};
//
//
// 	template<class T, class A>
// 	struct has_member_load_minimal {
// 		using save_type = get_member_save_minimal_t<A, T>;
//
// 		// Strategy: Check if a function matching the signature more or less exists
// 		// (allow anything like load_minimal(xxx) using AnyConvert, and then enforce that it has the correct signature using NoConvertAnyRef
// 		static constexpr bool exists = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert()); };
// 		static constexpr bool value = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>()); };
//
// 		static_assert(exists == value, "Vide detected different or invalid types in corresponding member " "load_minimal" " and " "save_minimal" " functions. \n " "the paramater to " "load_minimal" " must be the type (optionally: value, &, const& or &&) that " "save_minimal" " returns.");
// 	};
//
// 	template<class T, class A>
// 	struct has_member_load_minimal_versioned {
// 		using save_type = get_member_save_minimal_versioned_t<A, T>;
//
// 		// Strategy: Check if a function matching the signature more or less exists
// 		// (allow anything like load_minimal(xxx) using AnyConvert, and then enforce that it has the correct signature using NoConvertAnyRef
// 		static constexpr bool exists = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert(), detail::AnyConvert()); };
// 		static constexpr bool value = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>(), std::declval<uint32_t>()); };
//
// 		static_assert(exists == value, "Vide detected different or invalid types in corresponding member " "load_minimal_versioned" " and " "save_minimal_versioned" " functions. \n " "the paramater to " "load_minimal_versioned" " must be the type (optionally: value, &, const& or &&) that " "save_minimal_versioned" " returns.");
// 	};
//
// 	template<class T, class A>
// 	struct has_global_load_minimal {
// 		using save_type = get_global_save_minimal_t<A, T>;
//
// 		// Note that there should be an additional const check on load_minimal after the valid check,
// 		// but this currently interferes with many valid uses of minimal serialization.  It has been
// 		// removed (see #565 on github) and previously was:
// 		//
// 		// @code
// 		// static_assert( check::const_valid || !check::exists,
// 		// 	"Vide detected an invalid serialization type parameter in non-member " #test_name ".  "
// 		// 	#test_name " non-member functions must accept their serialization type by non-const reference" );
// 		// @endcode
//
// 		static constexpr bool exists = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert()); };
// 		static constexpr bool valid = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>()); };
// 		static constexpr bool const_valid = requires { load_minimal(std::declval<const A&>(), detail::NoConvertRef<T>(), detail::AnyConvert()); };
//
// 		static_assert(!exists || (valid && const_valid), "Vide detected different types in corresponding non-member " "load_minimal" " and " "save_minimal" " functions. \n " "the paramater to " "load_minimal" " must be a constant reference to the type that " "save_minimal" " returns.");
// 		static constexpr bool value = exists && valid && const_valid;
// 	};
//
// 	template<class T, class A>
// 	struct has_global_load_minimal_versioned {
// 		using save_type = get_global_save_minimal_versioned_t<A, T>;
//
// 		static constexpr bool exists = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert(), std::declval<uint32_t>()); };
// 		static constexpr bool valid = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>(), std::declval<uint32_t>()); };
// 		static constexpr bool const_valid = requires { load_minimal(std::declval<const A&>(), detail::NoConvertRef<T>(), detail::AnyConvert(), std::declval<uint32_t>()); };
//
// 		static_assert(!exists || (valid && const_valid), "Vide detected different types in corresponding non-member " "load_minimal_versioned" " and " "save_minimal_versioned" " functions. \n " "the paramater to " "load_minimal_versioned" " must be a constant reference to the type that " "save_minimal_versioned" " returns.");
// 		static constexpr bool value = exists && valid && const_valid;
// 	};
//
// 	// ######################################################################
// 	template <class T, class OutputArchive>
// 	struct has_invalid_output_versioning : std::integral_constant<bool,
// 			// (has_member_versioned_save<T, OutputArchive>::value && has_member_save<T, OutputArchive>::value) ||
// 			(has_global_versioned_save<T, OutputArchive>::value && has_global_save<T, OutputArchive>::value) ||
// 			(has_member_versioned_serialize<T, OutputArchive>::value && has_member_serialize<T, OutputArchive>::value) ||
// 			(has_global_versioned_serialize<T, OutputArchive>::value && has_global_serialize<T, OutputArchive>::value) ||
// 			// (has_member_save_minimal_versioned<T, OutputArchive>::value && has_member_save_minimal<T, OutputArchive>::value) ||
// 			(has_global_save_minimal_versioned<T, OutputArchive>::value && has_global_save_minimal<T, OutputArchive>::value)> {
// 	};
//
// 	// ######################################################################
// 	template <class T, class InputArchive>
// 	struct has_invalid_input_versioning : std::integral_constant<bool,
// 			(has_member_versioned_load<T, InputArchive>::value && has_member_load<T, InputArchive>::value) ||
// 			(has_global_versioned_load<T, InputArchive>::value && has_global_load<T, InputArchive>::value) ||
// 			(has_member_versioned_serialize<T, InputArchive>::value && has_member_serialize<T, InputArchive>::value) ||
// 			(has_global_versioned_serialize<T, InputArchive>::value && has_global_serialize<T, InputArchive>::value) ||
// 			(has_member_load_minimal_versioned<T, InputArchive>::value && has_member_load_minimal<T, InputArchive>::value) ||
// 			(has_global_load_minimal_versioned<T, InputArchive>::value && has_global_load_minimal<T, InputArchive>::value)> {
// 	};
// };
