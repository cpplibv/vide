/*! \file traits.hpp
    \brief Internal type trait support
    \ingroup Internal */
/*
  Copyright (c) 2013-2022, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <type_traits>
#include <typeindex>

#include <vide/access.hpp>
#include <vide/macros.hpp>


namespace vide {
namespace traits {

// -------------------------------------------------------------------------------------------------

using yes = std::true_type;
using no = std::false_type;

namespace detail {
// ######################################################################
//! Used to delay a static_assert until template instantiation
template <class T>
struct delay_static_assert : std::false_type {};

// ######################################################################
// SFINAE Helpers

//! Return type for SFINAE Enablers
enum class sfinae {};

// ######################################################################
// Helper functionality for boolean integral constants and Enable/DisableIf
template <bool H, bool ... T> struct meta_bool_and : std::integral_constant<bool, H && meta_bool_and<T...>::value> {};
template <bool B> struct meta_bool_and<B> : std::integral_constant<bool, B> {};

template <bool H, bool ... T> struct meta_bool_or : std::integral_constant<bool, H || meta_bool_or<T...>::value> {};
template <bool B> struct meta_bool_or<B> : std::integral_constant<bool, B> {};

// workaround needed due to bug in MSVC 2013, see
// http://connect.microsoft.com/VisualStudio/feedback/details/800231/c-11-alias-template-issue
template <bool ... Conditions>
struct EnableIfHelper : std::enable_if<meta_bool_and<Conditions...>::value, sfinae> {};

template <bool ... Conditions>
struct DisableIfHelper : std::enable_if<!meta_bool_or<Conditions...>::value, sfinae> {};
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

// ######################################################################
//! Provides a way to disable a function if conditions are met
/*! This is intended to be used in a near identical fashion to std::enable_if
	while being significantly easier to read at the cost of not allowing for as
	complicated of a condition.

	This will compile (allow the function) if every condition evaluates to false.
	This should be used with SFINAE to ensure that at least one other candidate
	function works when one fails due to a DisableIf.

	This should be used as the las template parameter to a function as
	an unnamed parameter with a default value of vide::traits::sfinae:

	@code{cpp}
	// using by making the last template argument variadic
	template <class T, DisableIf<std::is_same<T, bool>::value> = sfinae>
	void func(T t );
	@endcode

	This is often used in conjunction with EnableIf to form an enable/disable pair of
	overloads.

	Note that this performs a logical AND of all conditions, so you will need
	to construct more complicated requirements with this fact in mind.  If all conditions
	hold, the function will be disabled.

	@relates EnableIf
	@relates sfinae
	@tparam Conditions The conditions which will be logically ANDed to disable the function. */
template <bool ... Conditions>
using DisableIf = typename detail::DisableIfHelper<Conditions...>::type;

// ######################################################################
//! Used to convert a MAKE_HAS_XXX macro into a versioned variant
#define VIDE_MAKE_VERSIONED_TEST ,0

// ######################################################################
//! Creates a test for whether a non const member function exists
/*! This creates a class derived from std::integral_constant that will be true if
	the type has the proper member function for the given archive.

	@param name The name of the function to test for (e.g. serialize, load, save)
	@param test_name The name to give the test for the function being tested for (e.g. serialize, versioned_serialize)
	@param versioned Either blank or the macro VIDE_MAKE_VERSIONED_TEST */
#define VIDE_MAKE_HAS_MEMBER_TEST(name, test_name, versioned)                                                                     \
    namespace detail                                                                                                                    \
    {                                                                                                                                   \
      template <class T, class A>                                                                                                       \
      struct has_member_##name##_##versioned##_impl                                                                                     \
      {                                                                                                                                 \
        template <class TT, class AA>                                                                                                   \
        static auto test(int) -> decltype( vide::access::member_##name( std::declval<AA&>(), std::declval<TT&>() versioned ), yes()); \
        template <class, class>                                                                                                         \
        static no test(...);                                                                                                            \
        static const bool value = std::is_same<decltype(test<T, A>(0)), yes>::value;                                                    \
      };                                                                                                                                \
    } /* end namespace detail */                                                                                                        \
    template <class T, class A>                                                                                                         \
    struct has_member_##test_name : std::integral_constant<bool, detail::has_member_##name##_##versioned##_impl<T, A>::value> {}

// ######################################################################
//! Creates a test for whether a non const non-member function exists
/*! This creates a class derived from std::integral_constant that will be true if
	the type has the proper non-member function for the given archive. */
#define VIDE_MAKE_HAS_NON_MEMBER_TEST(test_name, func, versioned)                                                         \
    namespace detail                                                                                                            \
    {                                                                                                                           \
      template <class T, class A>                                                                                               \
      struct has_non_member_##test_name##_impl                                                                                  \
      {                                                                                                                         \
        template <class TT, class AA>                                                                                           \
        static auto test(int) -> decltype( func( std::declval<AA&>(), std::declval<TT&>() versioned ), yes());                  \
        template <class, class>                                                                                                 \
        static no test( ... );                                                                                                  \
        static const bool value = std::is_same<decltype( test<T, A>( 0 ) ), yes>::value;                                        \
      };                                                                                                                        \
    } /* end namespace detail */                                                                                                \
    template <class T, class A>                                                                                                 \
    struct has_non_member_##test_name : std::integral_constant<bool, detail::has_non_member_##test_name##_impl<T, A>::value> {}

// ######################################################################

// Member Serialize
VIDE_MAKE_HAS_MEMBER_TEST(serialize, serialize,);
// Member Serialize (versioned)
VIDE_MAKE_HAS_MEMBER_TEST(serialize, versioned_serialize, VIDE_MAKE_VERSIONED_TEST);
// Non Member Serialize
VIDE_MAKE_HAS_NON_MEMBER_TEST(serialize, VIDE_FUNCTION_NAME_SERIALIZE,);
// Non Member Serialize (versioned)
VIDE_MAKE_HAS_NON_MEMBER_TEST(versioned_serialize, VIDE_FUNCTION_NAME_SERIALIZE, VIDE_MAKE_VERSIONED_TEST);
// Member Load
VIDE_MAKE_HAS_MEMBER_TEST(load, load,);
// Member Load (versioned)
VIDE_MAKE_HAS_MEMBER_TEST(load, versioned_load, VIDE_MAKE_VERSIONED_TEST);
// Non Member Load
VIDE_MAKE_HAS_NON_MEMBER_TEST(load, VIDE_FUNCTION_NAME_LOAD,);
// Non Member Load (versioned)
VIDE_MAKE_HAS_NON_MEMBER_TEST(versioned_load, VIDE_FUNCTION_NAME_LOAD, VIDE_MAKE_VERSIONED_TEST);

// ######################################################################
#undef VIDE_MAKE_HAS_NON_MEMBER_TEST
#undef VIDE_MAKE_HAS_MEMBER_TEST

// ######################################################################
//! Creates a test for whether a member save function exists
/*! This creates a class derived from std::integral_constant that will be true if
	the type has the proper member function for the given archive.

	@param test_name The name to give the test (e.g. save or versioned_save)
	@param versioned Either blank or the macro VIDE_MAKE_VERSIONED_TEST */
#define VIDE_MAKE_HAS_MEMBER_SAVE_IMPL(test_name, versioned)                                                                  \
    namespace detail                                                                                                                \
    {                                                                                                                               \
    template <class T, class A>                                                                                                     \
    struct has_member_##test_name##_impl                                                                                            \
      {                                                                                                                             \
        template <class TT, class AA>                                                                                               \
        static auto test(int) -> decltype( vide::access::member_save( std::declval<AA&>(),                                        \
                                                                        std::declval<TT const &>() versioned ), yes());             \
        template <class, class> static no test(...);                                                                                \
        static const bool value = std::is_same<decltype(test<T, A>(0)), yes>::value;                                                \
                                                                                                                                    \
        template <class TT, class AA>                                                                                               \
        static auto test2(int) -> decltype( vide::access::member_save_non_const(                                                  \
                                              std::declval<AA &>(),                                                                 \
                                              std::declval<typename std::remove_const<TT>::type&>() versioned ), yes());            \
        template <class, class> static no test2(...);                                                                               \
        static const bool not_const_type = std::is_same<decltype(test2<T, A>(0)), yes>::value;                                      \
      };                                                                                                                            \
    } /* end namespace detail */

// ######################################################################
// Member Save
VIDE_MAKE_HAS_MEMBER_SAVE_IMPL(save,)

template <class T, class A>
struct has_member_save : std::integral_constant<bool, detail::has_member_save_impl<T, A>::value> {
	typedef typename detail::has_member_save_impl<T, A> check;
	static_assert(check::value || !check::not_const_type,
			"Vide detected a non-const save. \n "
			"save member functions must always be const");
};

// ######################################################################
// Member Save (versioned)
VIDE_MAKE_HAS_MEMBER_SAVE_IMPL(versioned_save, VIDE_MAKE_VERSIONED_TEST)

template <class T, class A>
struct has_member_versioned_save : std::integral_constant<bool, detail::has_member_versioned_save_impl<T, A>::value> {
	typedef typename detail::has_member_versioned_save_impl<T, A> check;
	static_assert(check::value || !check::not_const_type,
			"Vide detected a versioned non-const save. \n "
			"save member functions must always be const");
};

// ######################################################################
#undef VIDE_MAKE_HAS_MEMBER_SAVE_IMPL

// ######################################################################
//! Creates a test for whether a non-member save function exists
/*! This creates a class derived from std::integral_constant that will be true if
	the type has the proper non-member function for the given archive.

	@param test_name The name to give the test (e.g. save or versioned_save)
	@param versioned Either blank or the macro VIDE_MAKE_VERSIONED_TEST */
#define VIDE_MAKE_HAS_NON_MEMBER_SAVE_TEST(test_name, versioned)                                                       \
    namespace detail                                                                                                         \
    {                                                                                                                        \
      template <class T, class A>                                                                                            \
      struct has_non_member_##test_name##_impl                                                                               \
      {                                                                                                                      \
        template <class TT, class AA>                                                                                        \
        static auto test(int) -> decltype( VIDE_FUNCTION_NAME_SAVE(                                                        \
                                              std::declval<AA&>(),                                                           \
                                              std::declval<TT const &>() versioned ), yes());                                \
        template <class, class> static no test(...);                                                                         \
        static const bool value = std::is_same<decltype(test<T, A>(0)), yes>::value;                                         \
                                                                                                                             \
        template <class TT, class AA>                                                                                        \
        static auto test2(int) -> decltype( VIDE_FUNCTION_NAME_SAVE(                                                       \
                                              std::declval<AA &>(),                                                          \
                                              std::declval<typename std::remove_const<TT>::type&>() versioned ), yes());     \
        template <class, class> static no test2(...);                                                                        \
        static const bool not_const_type = std::is_same<decltype(test2<T, A>(0)), yes>::value;                               \
      };                                                                                                                     \
    } /* end namespace detail */                                                                                             \
                                                                                                                             \
    template <class T, class A>                                                                                              \
    struct has_non_member_##test_name : std::integral_constant<bool, detail::has_non_member_##test_name##_impl<T, A>::value> \
    {                                                                                                                        \
      using check = typename detail::has_non_member_##test_name##_impl<T, A>;                                                \
      static_assert( check::value || !check::not_const_type,                                                                 \
        "Vide detected a non-const type parameter in non-member " #test_name ". \n "                                       \
        #test_name " non-member functions must always pass their types as const" );                                          \
    };

// ######################################################################
// Non Member Save
VIDE_MAKE_HAS_NON_MEMBER_SAVE_TEST(save,)

// ######################################################################
// Non Member Save (versioned)
VIDE_MAKE_HAS_NON_MEMBER_SAVE_TEST(versioned_save, VIDE_MAKE_VERSIONED_TEST)

// ######################################################################
#undef VIDE_MAKE_HAS_NON_MEMBER_SAVE_TEST

// ######################################################################
//! Creates implementation details for whether a member save_minimal function exists
/*! This creates a class derived from std::integral_constant that will be true if
	the type has the proper member function for the given archive.

	@param test_name The name to give the test (e.g. save_minimal or versioned_save_minimal)
	@param versioned Either blank or the macro VIDE_MAKE_VERSIONED_TEST */
#define VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_IMPL(test_name, versioned)                           \
    namespace detail                                                                           \
    {                                                                                          \
      template <class T, class A>                                                              \
      struct has_member_##test_name##_impl                                                     \
      {                                                                                        \
        template <class TT, class AA>                                                          \
        static auto test(int) -> decltype( vide::access::member_save_minimal(                  \
              std::declval<AA const &>(),                                                      \
              std::declval<TT const &>() versioned ), yes());                                  \
        template <class, class> static no test(...);                                           \
        static const bool value = std::is_same<decltype(test<T, A>(0)), yes>::value;           \
                                                                                               \
        template <class TT, class AA>                                                          \
        static auto test2(int) -> decltype( vide::access::member_save_minimal_non_const(       \
              std::declval<AA const &>(),                                                      \
              std::declval<typename std::remove_const<TT>::type&>() versioned ), yes());       \
        template <class, class> static no test2(...);                                          \
        static const bool not_const_type = std::is_same<decltype(test2<T, A>(0)), yes>::value; \
                                                                                               \
        static const bool valid = value || !not_const_type;                                    \
      };                                                                                       \
    } /* end namespace detail */

// ######################################################################
//! Creates helpers for minimal save functions
/*! The get_member_*_type structs allow access to the return type of a save_minimal,
	assuming that the function actually exists.  If the function does not
	exist, the type will be void.

	@param test_name The name to give the test (e.g. save_minimal or versioned_save_minimal)
	@param versioned Either blank or the macro VIDE_MAKE_VERSIONED_TEST */
#define VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_HELPERS_IMPL(test_name, versioned)                           \
    namespace detail                                                                                         \
    {                                                                                                        \
      template <class T, class A, bool Valid>                                                                \
      struct get_member_##test_name##_type { using type = void; };                                           \
                                                                                                             \
      template <class T, class A>                                                                            \
      struct get_member_##test_name##_type<T, A, true>                                                       \
      {                                                                                                      \
        using type = decltype( vide::access::member_save_minimal( std::declval<A const &>(),               \
                                                                    std::declval<T const &>() versioned ) ); \
      };                                                                                                     \
    } /* end namespace detail */

// ######################################################################
//! Creates a test for whether a member save_minimal function exists
/*! This creates a class derived from std::integral_constant that will be true if
	the type has the proper member function for the given archive.

	@param test_name The name to give the test (e.g. save_minimal or versioned_save_minimal) */
#define VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_TEST(test_name)                                                            \
    template <class T, class A>                                                                                      \
    struct has_member_##test_name : std::integral_constant<bool, detail::has_member_##test_name##_impl<T, A>::value> \
    {                                                                                                                \
      using check = typename detail::has_member_##test_name##_impl<T, A>;                                            \
      static_assert( check::valid,                                                                                   \
        "Vide detected a non-const member " #test_name ". \n "                                                       \
        #test_name " member functions must always be const" );                                                       \
                                                                                                                     \
      using type = typename detail::get_member_##test_name##_type<T, A, check::value>::type;                         \
    };

// ######################################################################
// Member Save Minimal
VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_IMPL(save_minimal,)
VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_HELPERS_IMPL(save_minimal,)
VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_TEST(save_minimal)

// ######################################################################
// Member Save Minimal (versioned)
VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_IMPL(versioned_save_minimal, VIDE_MAKE_VERSIONED_TEST)
VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_HELPERS_IMPL(versioned_save_minimal, VIDE_MAKE_VERSIONED_TEST)
VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_TEST(versioned_save_minimal)

// ######################################################################
#undef VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_IMPL
#undef VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_HELPERS_IMPL
#undef VIDE_MAKE_HAS_MEMBER_SAVE_MINIMAL_TEST

// ######################################################################
//! Creates a test for whether a non-member save_minimal function exists
/*! This creates a class derived from std::integral_constant that will be true if
	the type has the proper member function for the given archive.

	@param test_name The name to give the test (e.g. save_minimal or versioned_save_minimal)
	@param versioned Either blank or the macro VIDE_MAKE_VERSIONED_TEST */
#define VIDE_MAKE_HAS_NON_MEMBER_SAVE_MINIMAL_TEST(test_name, versioned)                                                     \
    namespace detail                                                                                                         \
    {                                                                                                                        \
      template <class T, class A>                                                                                            \
      struct has_non_member_##test_name##_impl                                                                               \
      {                                                                                                                      \
        template <class TT, class AA>                                                                                        \
        static auto test(int) -> decltype( VIDE_FUNCTION_NAME_SAVE_MINIMAL(                                                  \
              std::declval<AA const &>(),                                                                                    \
              std::declval<TT const &>() versioned ), yes());                                                                \
        template <class, class> static no test(...);                                                                         \
        static const bool value = std::is_same<decltype(test<T, A>(0)), yes>::value;                                         \
                                                                                                                             \
        template <class TT, class AA>                                                                                        \
        static auto test2(int) -> decltype( VIDE_FUNCTION_NAME_SAVE_MINIMAL(                                                 \
              std::declval<AA const &>(),                                                                                    \
              std::declval<typename std::remove_const<TT>::type&>() versioned ), yes());                                     \
        template <class, class> static no test2(...);                                                                        \
        static const bool not_const_type = std::is_same<decltype(test2<T, A>(0)), yes>::value;                               \
                                                                                                                             \
        static const bool valid = value || !not_const_type;                                                                  \
      };                                                                                                                     \
                                                                                                                             \
      template <class T, class A, bool Valid>                                                                                \
      struct get_non_member_##test_name##_type { using type = void; };                                                       \
                                                                                                                             \
      template <class T, class A>                                                                                            \
      struct get_non_member_##test_name##_type <T, A, true>                                                                  \
      {                                                                                                                      \
        using type = decltype( VIDE_FUNCTION_NAME_SAVE_MINIMAL( std::declval<A const &>(),                                   \
                                                                  std::declval<T const &>() versioned ) );                   \
      };                                                                                                                     \
    } /* end namespace detail */                                                                                             \
                                                                                                                             \
    template <class T, class A>                                                                                              \
    struct has_non_member_##test_name : std::integral_constant<bool, detail::has_non_member_##test_name##_impl<T, A>::value> \
    {                                                                                                                        \
      using check = typename detail::has_non_member_##test_name##_impl<T, A>;                                                \
      static_assert( check::valid,                                                                                           \
        "Vide detected a non-const type parameter in non-member " #test_name ". \n "                                         \
        #test_name " non-member functions must always pass their types as const" );                                          \
                                                                                                                             \
      using type = typename detail::get_non_member_##test_name##_type<T, A, check::value>::type;                             \
    };

// ######################################################################
// Non-Member Save Minimal
VIDE_MAKE_HAS_NON_MEMBER_SAVE_MINIMAL_TEST(save_minimal,)

// ######################################################################
// Non-Member Save Minimal (versioned)
VIDE_MAKE_HAS_NON_MEMBER_SAVE_MINIMAL_TEST(versioned_save_minimal, VIDE_MAKE_VERSIONED_TEST)

// ######################################################################
#undef VIDE_MAKE_HAS_NON_MEMBER_SAVE_MINIMAL_TEST

// ######################################################################
// Load Minimal Utilities
namespace detail {
//! Used to help strip away conversion wrappers
/*! If someone writes a non-member load/save minimal function that accepts its
	parameter as some generic template type and needs to perform trait checks
	on that type, our NoConvert wrappers will interfere with this.  Using
	the struct strip_minmal, users can strip away our wrappers to get to
	the underlying type, allowing traits to work properly */
struct NoConvertBase {};

//! A struct that prevents implicit conversion
/*! Any type instantiated with this struct will be unable to implicitly convert
	to another type.  Is designed to only allow conversion to const Source&.

	Allows:
		      Source  -> const Target&

	@tparam Source the type of the original source */
template <class Source>
struct NoConvertConstRef : NoConvertBase {
	using type = Source; //!< Used to get underlying type easily

	template <class Dest, class = typename std::enable_if<std::is_same<Source, Dest>::value>::type>
	operator Dest() = delete;

	//! only allow conversion if the types are the same and we are converting into a const reference
	template <class Dest, class = typename std::enable_if<std::is_same<Source, Dest>::value>::type>
	operator const Dest&();
};

//! A struct that prevents implicit conversion
/*! Any type instantiated with this struct will be unable to implicitly convert
	to another type.  Is designed to only allow conversion to const Source&.

	Allows:
		const Source& -> const Target&
		const Source  -> const Target&
		      Source& -> const Target&
		      Source  -> const Target&

	@tparam Source the type of the original source */
template <class Source>
struct NoConvertConstRefAllowCVREFSource : NoConvertBase {
	using type = Source; //!< Used to get underlying type easily

	template <class Dest>
		requires std::is_same_v<std::remove_cvref_t<Source>, std::remove_cvref_t<Dest>>
	operator Dest() = delete;

	//! only allow conversion if the types are the same and we are converting into a const reference
	template <class Dest>
		requires std::is_same_v<std::remove_cvref_t<Source>, std::remove_cvref_t<Dest>>
	operator const Dest&();
};

//! A struct that prevents implicit conversion
/*! Any type instantiated with this struct will be unable to implicitly convert
	to another type.  Is designed to only allow conversion to Source &.

	@tparam Source the type of the original source */
template <class Source>
struct NoConvertRef : NoConvertBase {
	using type = Source; //!< Used to get underlying type easily

	template <class Dest, class = typename std::enable_if<std::is_same<Source, Dest>::value>::type>
	operator Dest() = delete;

#ifdef __clang__
	template <class Dest, class = typename std::enable_if<std::is_same<Source, Dest>::value>::type>
	operator Dest const & () = delete;
#endif // __clang__

	//! only allow conversion if the types are the same and we are converting into a const reference
	template <class Dest, class = typename std::enable_if<std::is_same<Source, Dest>::value>::type>
	operator Dest&();
};

//! A struct that prevents implicit conversion to a different type
template <class Source>
	requires (!std::is_void_v<Source>)
struct NoConvertAnyRef : NoConvertBase {
	using type = Source; //!< Used to get underlying type easily

	operator Source&();
	operator Source&&();
	operator const Source&() const;
};

//! A type that can implicitly convert to anything else
struct AnyConvert {
	template <class Dest>
	operator Dest&();

	template <class Dest>
	operator Dest&&();

	template <class Dest>
	operator const Dest&() const;
};

} // namespace detail ------------------------------------------------------------------------------

// =================================================================================================
// =================================================================================================

//// NOTE: The get_output_from_input only used for save_minimal/load_minimal
////			and there only to determine the return type of save_minimal
//
//template <typename Archive>
//using get_output_from_input = typename Archive::ArchiveOutput;
//
//// -------------------------------------------------------------------------------------------------
//
//template <typename Archive, typename T>
//using get_member_save_minimal_type = typename has_member_save_minimal<T, get_output_from_input<Archive>>::type;
//
//template <typename Archive, typename T>
//using get_non_member_save_minimal_type = typename has_non_member_save_minimal<T, get_output_from_input<Archive>>::type;
//
//template <typename Archive, typename T>
//using get_member_versioned_save_minimal_type = typename has_member_versioned_save_minimal<T, get_output_from_input<Archive>>::type;
//
//template <typename Archive, typename T>
//using get_non_member_versioned_save_minimal_type = typename has_non_member_versioned_save_minimal<T, get_output_from_input<Archive>>::type;

template <typename Archive, typename T>
using get_member_save_minimal_type = std::remove_cvref_t<typename has_member_save_minimal<T, Archive>::type>;

template <typename Archive, typename T>
using get_non_member_save_minimal_type = std::remove_cvref_t<typename has_non_member_save_minimal<T, Archive>::type>;

template <typename Archive, typename T>
using get_member_versioned_save_minimal_type = std::remove_cvref_t<typename has_member_versioned_save_minimal<T, Archive>::type>;

template <typename Archive, typename T>
using get_non_member_versioned_save_minimal_type = std::remove_cvref_t<typename has_non_member_versioned_save_minimal<T, Archive>::type>;

// =================================================================================================

// TODO P3: Split has_ functions into has_ and diagnose_. has only asks one questions and has no static assert, while diagnose_ tries to check everything. diagnose_ only gets called if a serialize funtion is not found.
// template<class T, class A>
// struct has_member_versioned_load_minimal { ...
// template<class T, class A>
// struct diagnose_member_versioned_load_minimal { ...

// TODO P2: Modernize the rest of these

template<class T, class A>
struct has_member_load_minimal {
	using save_type = get_member_save_minimal_type<A, T>;

	// Strategy: Check if a function matching the signature more or less exists
	// (allow anything like load_minimal(xxx) using AnyConvert, and then enforce that it has the correct signature using NoConvertAnyRef
	static constexpr bool exists = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert()); };
	static constexpr bool value = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>()); };

	static_assert(exists == value, "Vide detected different or invalid types in corresponding member " "load_minimal" " and " "save_minimal" " functions. \n " "the paramater to " "load_minimal" " must be the type (optionally: value, &, const& or &&) that " "save_minimal" " returns.");
};

template<class T, class A>
struct has_member_versioned_load_minimal {
	using save_type = get_member_versioned_save_minimal_type<A, T>;

	// Strategy: Check if a function matching the signature more or less exists
	// (allow anything like load_minimal(xxx) using AnyConvert, and then enforce that it has the correct signature using NoConvertAnyRef
	static constexpr bool exists = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert(), detail::AnyConvert()); };
	static constexpr bool value = requires { access::member_load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>(), std::declval<uint32_t>()); };

	static_assert(exists == value, "Vide detected different or invalid types in corresponding member " "versioned_load_minimal" " and " "versioned_save_minimal" " functions. \n " "the paramater to " "versioned_load_minimal" " must be the type (optionally: value, &, const& or &&) that " "versioned_save_minimal" " returns.");
};

template<class T, class A>
struct has_non_member_load_minimal {
	using save_type = get_non_member_save_minimal_type<A, T>;

	// Note that there should be an additional const check on load_minimal after the valid check,
	// but this currently interferes with many valid uses of minimal serialization.  It has been
	// removed (see #565 on github) and previously was:
	//
	// @code
	// static_assert( check::const_valid || !check::exists,
	// 	"Vide detected an invalid serialization type parameter in non-member " #test_name ".  "
	// 	#test_name " non-member functions must accept their serialization type by non-const reference" );
	// @endcode

	static constexpr bool exists = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert()); };
	static constexpr bool valid = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>()); };
	static constexpr bool const_valid = requires { load_minimal(std::declval<const A&>(), detail::NoConvertRef<T>(), detail::AnyConvert()); };

	static_assert(!exists || (valid && const_valid), "Vide detected different types in corresponding non-member " "load_minimal" " and " "save_minimal" " functions. \n " "the paramater to " "load_minimal" " must be a constant reference to the type that " "save_minimal" " returns.");
	static constexpr bool value = exists && valid && const_valid;
};

template<class T, class A>
struct has_non_member_versioned_load_minimal {
	using save_type = get_non_member_versioned_save_minimal_type<A, T>;

	static constexpr bool exists = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::AnyConvert(), std::declval<uint32_t>()); };
	static constexpr bool valid = requires { load_minimal(std::declval<const A&>(), std::declval<T&>(), detail::NoConvertAnyRef<save_type>(), std::declval<uint32_t>()); };
	static constexpr bool const_valid = requires { load_minimal(std::declval<const A&>(), detail::NoConvertRef<T>(), detail::AnyConvert(), std::declval<uint32_t>()); };

	static_assert(!exists || (valid && const_valid), "Vide detected different types in corresponding non-member " "versioned_load_minimal" " and " "versioned_save_minimal" " functions. \n " "the paramater to " "versioned_load_minimal" " must be a constant reference to the type that " "versioned_save_minimal" " returns.");
	static constexpr bool value = exists && valid && const_valid;
};

// =================================================================================================
// ######################################################################
// End of serialization existence tests
#undef VIDE_MAKE_VERSIONED_TEST

// ######################################################################
template <class T, class InputArchive, class OutputArchive>
struct has_member_split : std::integral_constant<bool,
		(has_member_load<T, InputArchive>::value && has_member_save<T, OutputArchive>::value) ||
		(has_member_versioned_load<T, InputArchive>::value && has_member_versioned_save<T, OutputArchive>::value)> {
};

// ######################################################################
template <class T, class InputArchive, class OutputArchive>
struct has_non_member_split : std::integral_constant<bool,
		(has_non_member_load<T, InputArchive>::value && has_non_member_save<T, OutputArchive>::value) ||
		(has_non_member_versioned_load<T, InputArchive>::value && has_non_member_versioned_save<T, OutputArchive>::value)> {
};

// ######################################################################
template <class T, class OutputArchive>
struct has_invalid_output_versioning : std::integral_constant<bool,
		(has_member_versioned_save<T, OutputArchive>::value && has_member_save<T, OutputArchive>::value) ||
		(has_non_member_versioned_save<T, OutputArchive>::value && has_non_member_save<T, OutputArchive>::value) ||
		(has_member_versioned_serialize<T, OutputArchive>::value && has_member_serialize<T, OutputArchive>::value) ||
		(has_non_member_versioned_serialize<T, OutputArchive>::value && has_non_member_serialize<T, OutputArchive>::value) ||
		(has_member_versioned_save_minimal<T, OutputArchive>::value && has_member_save_minimal<T, OutputArchive>::value) ||
		(has_non_member_versioned_save_minimal<T, OutputArchive>::value && has_non_member_save_minimal<T, OutputArchive>::value)> {
};

// ######################################################################
template <class T, class InputArchive>
struct has_invalid_input_versioning : std::integral_constant<bool,
		(has_member_versioned_load<T, InputArchive>::value && has_member_load<T, InputArchive>::value) ||
		(has_non_member_versioned_load<T, InputArchive>::value && has_non_member_load<T, InputArchive>::value) ||
		(has_member_versioned_serialize<T, InputArchive>::value && has_member_serialize<T, InputArchive>::value) ||
		(has_non_member_versioned_serialize<T, InputArchive>::value && has_non_member_serialize<T, InputArchive>::value) ||
		(has_member_versioned_load_minimal<T, InputArchive>::value && has_member_load_minimal<T, InputArchive>::value) ||
		(has_non_member_versioned_load_minimal<T, InputArchive>::value && has_non_member_load_minimal<T, InputArchive>::value)> {
};

// ######################################################################
// detects if a type has any active minimal output serialization
template <class T, class OutputArchive>
struct has_minimal_output_serialization : std::integral_constant<bool,
		has_member_save_minimal<T, OutputArchive>::value ||
		has_non_member_save_minimal<T, OutputArchive>::value ||
		has_member_versioned_save_minimal<T, OutputArchive>::value ||
		has_non_member_versioned_save_minimal<T, OutputArchive>::value> {
};

// ######################################################################
// detects if a type has any active minimal input serialization
template <class T, class InputArchive>
struct has_minimal_input_serialization : std::integral_constant<bool,
		has_member_load_minimal<T, InputArchive>::value ||
		has_non_member_load_minimal<T, InputArchive>::value ||
		has_member_versioned_load_minimal<T, InputArchive>::value ||
		has_non_member_versioned_load_minimal<T, InputArchive>::value> {
};

// ######################################################################
namespace detail {
//! The number of output serialization functions available
/*! If specialization is being used, we'll count only those; otherwise we'll count everything */
template <class T, class OutputArchive>
struct count_output_serializers : std::integral_constant<int,
		has_member_save<T, OutputArchive>::value +
		has_non_member_save<T, OutputArchive>::value +
		has_member_serialize<T, OutputArchive>::value +
		has_non_member_serialize<T, OutputArchive>::value +
		has_member_save_minimal<T, OutputArchive>::value +
		has_non_member_save_minimal<T, OutputArchive>::value +
		/*-versioned---------------------------------------------------------*/
		has_member_versioned_save<T, OutputArchive>::value +
		has_non_member_versioned_save<T, OutputArchive>::value +
		has_member_versioned_serialize<T, OutputArchive>::value +
		has_non_member_versioned_serialize<T, OutputArchive>::value +
		has_member_versioned_save_minimal<T, OutputArchive>::value +
		has_non_member_versioned_save_minimal<T, OutputArchive>::value> {
};

}

template <class T, class OutputArchive>
struct is_output_serializable : std::integral_constant<bool,
		detail::count_output_serializers<T, OutputArchive>::value == 1> {
};

// ######################################################################
namespace detail {
//! The number of input serialization functions available
/*! If specialization is being used, we'll count only those; otherwise we'll count everything */
template <class T, class InputArchive>
struct count_input_serializers : std::integral_constant<int,
		has_member_load<T, InputArchive>::value +
		has_non_member_load<T, InputArchive>::value +
		has_member_serialize<T, InputArchive>::value +
		has_non_member_serialize<T, InputArchive>::value +
		has_member_load_minimal<T, InputArchive>::value +
		has_non_member_load_minimal<T, InputArchive>::value +
		/*-versioned---------------------------------------------------------*/
		has_member_versioned_load<T, InputArchive>::value +
		has_non_member_versioned_load<T, InputArchive>::value +
		has_member_versioned_serialize<T, InputArchive>::value +
		has_non_member_versioned_serialize<T, InputArchive>::value +
		has_member_versioned_load_minimal<T, InputArchive>::value +
		has_non_member_versioned_load_minimal<T, InputArchive>::value> {
};

}

template <class T, class InputArchive>
struct is_input_serializable : std::integral_constant<bool,
		detail::count_input_serializers<T, InputArchive>::value == 1> {
};

// ######################################################################
// Base Class Support
namespace detail {
struct base_class_id {
	template <class T>
	base_class_id(T const* const t) :
			type(typeid(T)),
			ptr(t),
			hash(std::hash<std::type_index>()(typeid(T)) ^ (std::hash<const void*>()(t) << 1)) {}

	bool operator==(base_class_id const& other) const { return (type == other.type) && (ptr == other.ptr); }

	std::type_index type;
	const void* ptr;
	size_t hash;
};

struct base_class_id_hash { size_t operator()(base_class_id const& id) const { return id.hash; }};
} // namespace detail

namespace detail {
//! Common base type for base class casting
struct BaseCastBase {};

template <class>
struct get_base_class;

template <template <typename> class Cast, class Base>
struct get_base_class<Cast<Base>> {
	using type = Base;
};

//! Base class cast, behave as the test
template <class Cast, template <class, class> class Test, class Archive,
		bool IsBaseCast = std::is_base_of<BaseCastBase, Cast>::value>
struct has_minimal_base_class_serialization_impl : Test<typename get_base_class<Cast>::type, Archive> {};

//! Not a base class cast
template <class Cast, template <class, class> class Test, class Archive>
struct has_minimal_base_class_serialization_impl<Cast, Test, Archive, false> : std::false_type {};
}

//! Checks to see if the base class used in a cast has a minimal serialization
/*! @tparam Cast Either base_class or virtual_base_class wrapped type
	@tparam Test A has_minimal test (for either input or output)
	@tparam Archive The archive to use with the test */
template <class Cast, template <class, class> class Test, class Archive>
struct has_minimal_base_class_serialization : detail::has_minimal_base_class_serialization_impl<Cast, Test, Archive> {};

// ######################################################################
//! Extracts the true type from something possibly wrapped in a vide NoConvert
/*! Internally vide uses some wrapper classes to test the validity of non-member
	minimal load and save functions.  This can interfere with user type traits on
	templated load and save minimal functions.  To get to the correct underlying type,
	users should use strip_minimal when performing any enable_if type type trait checks.

	See the enum serialization in types/common.hpp for an example of using this */
template <class T, bool IsCerealMinimalTrait = std::is_base_of<detail::NoConvertBase, T>::value>
struct strip_minimal {
	using type = T;
};

//! Specialization for types wrapped in a NoConvert
template <class T>
struct strip_minimal<T, true> {
	using type = typename T::type;
};

// ######################################################################
//! Determines whether the class T can be default constructed by vide::access
template <class T>
struct is_default_constructible {
	template <class TT>
	static auto test(int) -> decltype(vide::access::construct<TT>(), yes());
	template <class>
	static no test(...);
	static const bool value = std::is_same<decltype(test<T>(0)), yes>::value;
};

// ######################################################################
//namespace detail {
////! Removes all qualifiers and minimal wrappers from an archive
//template <class A>
//using decay_archive = typename std::decay<typename strip_minimal<A>::type>::type;
//}
//
////! Checks if the provided archive type is equal to some vide archive type
///*! This automatically does things such as std::decay and removing any other wrappers that may be
//	on the Archive template parameter.
//
//	Example use:
//	@code{cpp}
//	// example use to disable a serialization function
//	template <class Archive, EnableIf<vide::traits::is_same_archive<Archive, vide::BinaryOutputArchive>::value> = sfinae>
//	void save( Archive & ar, MyType const & mt );
//	@endcode */
//template <class ArchiveT, class CerealArchiveT>
//struct is_same_archive : std::integral_constant<bool,
//		std::is_same<detail::decay_archive<ArchiveT>, CerealArchiveT>::value> {
//};

} // namespace traits

// =================================================================================================
// =================================================================================================
// =================================================================================================

template <typename T>
concept binary_serializable_type = std::is_arithmetic_v<T> || std::is_same_v<T, std::byte>;

template <typename T>
concept Void = std::is_void_v<T>;

template <typename T>
concept NotVoid = not std::is_void_v<T>;

//template <typename Archive, typename T>
//struct diagnose_type {
//	// ALL THE DEBUG INFORMATION HERE, only instantiate if primary serialization_traits signaled failure
//};

template <typename Archive, typename T>
struct serialization_traits {

	// --- Non-Versioned

	static constexpr bool has_serialize_member = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SERIALIZE(ar) } -> Void;
	};

	static constexpr bool has_serialize_global = requires (Archive& ar, T& var) {
		{ VIDE_FUNCTION_NAME_SERIALIZE(ar, var) } -> Void;
	};

	static constexpr bool has_save_member = requires (Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE(ar) } -> Void;
	};
	static constexpr bool has_load_member = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_LOAD(ar) } -> Void;
	};

	static constexpr bool has_save_global = requires (Archive& ar, const T& var) {
		{ VIDE_FUNCTION_NAME_SAVE(ar, var) } -> Void;
	};
	static constexpr bool has_load_global = requires (Archive& ar, T& var) {
		{ VIDE_FUNCTION_NAME_LOAD(ar, var) } -> Void;
	};

	static constexpr bool has_save_minimal_member = requires (const Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar) } -> NotVoid;
	};
	static constexpr bool has_load_minimal_member =
			requires (const Archive& ar, const T& cvar, T& var) {
				{ var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, cvar.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar)) } -> Void;
			} ||
			requires (const Archive& ar, const T& cvar, T& var) {
				{ var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, cvar.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, 0)) } -> Void;
			};

	static constexpr bool has_save_minimal_global = requires (const Archive& ar, const T& var) {
		{ VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, var) } -> NotVoid;
	};
	static constexpr bool has_load_minimal_global =
			requires (const Archive& ar, const T& cvar, T& var) {
				{ VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, var, VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, cvar)) } -> Void;
			} ||
			requires (const Archive& ar, const T& cvar, T& var) {
				{ VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, var, VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, cvar, 0)) } -> Void;
			};

	// --- Versioned

	static constexpr bool has_serialize_member_versioned = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_SERIALIZE(ar, 0) } -> Void;
	};

	static constexpr bool has_serialize_global_versioned = requires (Archive& ar, T& var) {
		{ VIDE_FUNCTION_NAME_SERIALIZE(ar, var, 0) } -> Void;
	};

	static constexpr bool has_save_member_versioned = requires (Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE(ar, 0) } -> Void;
	};
	static constexpr bool has_load_member_versioned = requires (Archive& ar, T& var) {
		{ var.VIDE_FUNCTION_NAME_LOAD(ar, 0) } -> Void;
	};

	static constexpr bool has_save_global_versioned = requires (Archive& ar, const T& var) {
		{ VIDE_FUNCTION_NAME_SAVE(ar, var, 0) } -> Void;
	};
	static constexpr bool has_load_global_versioned = requires (Archive& ar, T& var) {
		{ VIDE_FUNCTION_NAME_LOAD(ar, var, 0) } -> Void;
	};

	static constexpr bool has_save_minimal_member_versioned = requires (const Archive& ar, const T& var) {
		{ var.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, 0) } -> NotVoid;
	};
	static constexpr bool has_load_minimal_member_versioned =
			requires (const Archive& ar, const T& cvar, T& var) {
				{ var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, cvar.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, 0), 0) } -> Void;
			} ||
			requires (const Archive& ar, const T& cvar, T& var) {
				{ var.VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, cvar.VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar), 0) } -> Void;
			};

	static constexpr bool has_save_minimal_global_versioned = requires (const Archive& ar, const T& var) {
		{ VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, var, 0) } -> NotVoid;
	};
	static constexpr bool has_load_minimal_global_versioned =
			requires (const Archive& ar, const T& cvar, T& var) {
				{ VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, var, VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, cvar, 0), 0) } -> Void;
			} ||
			requires (const Archive& ar, const T& cvar, T& var) {
				{ VIDE_FUNCTION_NAME_LOAD_MINIMAL(ar, var, VIDE_FUNCTION_NAME_SAVE_MINIMAL(ar, cvar), 0) } -> Void;
			};

	// --- Conclusions

	static constexpr int count_output_serializers =
			has_serialize_member +
			has_serialize_global +
			has_save_member +
			has_save_global +
			has_save_minimal_member +
			has_save_minimal_global +
			has_serialize_member_versioned +
			has_serialize_global_versioned +
			has_save_member_versioned +
			has_save_global_versioned +
			has_save_minimal_member_versioned +
			has_save_minimal_global_versioned;

	static constexpr int count_input_serializers =
			has_serialize_member +
			has_serialize_global +
			has_load_member +
			has_load_global +
			has_load_minimal_member +
			has_load_minimal_global +
			has_serialize_member_versioned +
			has_serialize_global_versioned +
			has_load_member_versioned +
			has_load_global_versioned +
			has_load_minimal_member_versioned +
			has_load_minimal_global_versioned;

	static constexpr bool is_input_serializable = count_input_serializers == 1;
	static constexpr bool is_output_serializable = count_output_serializers == 1;
};

// =================================================================================================
// =================================================================================================
// =================================================================================================
// -------------------------------------------------------------------------------------------------

} // namespace vide
