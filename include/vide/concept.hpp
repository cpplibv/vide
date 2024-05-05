// Created by Vader on 2022.02.14..

#pragma once

// std
#include <concepts>


namespace vide {

// -------------------------------------------------------------------------------------------------

template <typename T>
concept arithmetic = std::integral<T> || std::floating_point<T>;

template <typename T>
concept binary_serializable_type = std::is_arithmetic_v<T> || std::is_same_v<T, std::byte>;

template <typename T>
concept Void = std::is_void_v<T>;

template <typename T>
concept NotVoid = not std::is_void_v<T>;

// //! Used to help strip away conversion wrappers
// /*! If someone writes a non-member load/save minimal function that accepts its
// 	parameter as some generic template type and needs to perform trait checks
// 	on that type, our NoConvert wrappers will interfere with this.  Using
// 	the struct strip_minmal, users can strip away our wrappers to get to
// 	the underlying type, allowing traits to work properly */
// struct no_convert_base {};
//
// /// Helper struct that prevents implicit conversion to a different type
// template <class Source>
// struct no_convert_ref : no_convert_base {
// // 	template <typename Dest> requires std::is_same_v<Source, Dest>
// // 	explicit(false) operator Dest() = delete;
// // #ifdef __clang__
// // 	template <typename Dest> requires std::is_same_v<Source, Dest>
// // 	explicit(false) operator const Dest&() = delete;
// // #endif // __clang__
// //
// // 	template <typename Dest> requires std::is_same_v<Source, Dest>
// // 	explicit(false) operator Dest&();
//
// 	explicit(false) operator Source() = delete;
// 	explicit(false) operator Source&();
// };
//
// /// Helper struct that prevents implicit conversion to a different type
// template <class Source>
// struct no_convert_const_ref : no_convert_base {
// 	// template <typename Dest> requires std::is_same_v<Source, Dest>
// 	// explicit(false) operator Dest() = delete;
// 	//
// 	// template <typename Dest> requires std::is_same_v<Source, Dest>
// 	// explicit(false) operator const Dest&();
// 	explicit(false) operator Source() = delete;
// 	explicit(false) operator const Source&();
// };
//
// //! A struct that prevents implicit conversion to a different type
// template <class Source>
// struct no_convert_any_ref : no_convert_base {
// 	// template <typename Dest> requires std::is_same_v<Source, Dest>
// 	// explicit(false) operator Dest&();
// 	// template <typename Dest> requires std::is_same_v<Source, Dest>
// 	// explicit(false) operator Dest&&();
// 	// template <typename Dest> requires std::is_same_v<Source, Dest>
// 	// explicit(false) operator const Dest&();
// 	explicit(false) operator Source&();
// 	explicit(false) operator Source&&();
// 	explicit(false) operator const Source&();
// };
//
// //! A type that can implicitly convert to anything else
// struct any_convert {
// 	template <typename Dest>
// 	explicit(false) operator Dest&();
//
// 	template <typename Dest>
// 	explicit(false) operator Dest&&();
//
// 	template <typename Dest>
// 	explicit(false) operator const Dest&() const;
// };

} // namespace vide --------------------------------------------------------------------------------
