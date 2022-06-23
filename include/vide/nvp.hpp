#pragma once

#include <string>
#include <type_traits>
#include <utility>


namespace vide {

// -------------------------------------------------------------------------------------------------

namespace detail {

struct NameValuePairCore {}; //!< Traits struct for NVPs

} // namespace detail

// -------------------------------------------------------------------------------------------------

//! For holding name value pairs
/*! This pairs a name (some string) with some value such that an archive
	can potentially take advantage of the pairing.

	In serialization functions, NameValuePairs are usually created like so:
	@code{.cpp}
	struct MyStruct
	{
	  int a, b, c, d, e;

	  template<class Archive>
	  void serialize(Archive & archive)
	  {
		archive( VIDE_NVP(a),
				 VIDE_NVP(b),
				 VIDE_NVP(c),
				 VIDE_NVP(d),
				 VIDE_NVP(e) );
	  }
	};
	@endcode

	Alternatively, you can give you data members custom names like so:
	@code{.cpp}
	struct MyStruct
	{
	  int a, b, my_embarrassing_variable_name, d, e;

	  template<class Archive>
	  void serialize(Archive & archive)
	  {
		archive( VIDE_NVP(a),
				 VIDE_NVP(b),
				 vide::make_nvp("var", my_embarrassing_variable_name) );
				 VIDE_NVP(d),
				 VIDE_NVP(e) );
	  }
	};
	@endcode

	There is a slight amount of overhead to creating NameValuePairs, so there
	is a third method which will elide the names when they are not used by
	the Archive:

	@code{.cpp}
	struct MyStruct
	{
	  int a, b;

	  template<class Archive>
	  void serialize(Archive & archive)
	  {
		archive( vide::make_nvp<Archive>(a),
				 vide::make_nvp<Archive>(b) );
	  }
	};
	@endcode

	This third method is generally only used when providing generic type
	support.  Users writing their own serialize functions will normally
	explicitly control whether they want to use NVPs or not.

	@internal */
template <class T>
class NameValuePair : detail::NameValuePairCore {
private:
	// prevent nested nvps
	static_assert(!std::is_base_of_v<detail::NameValuePairCore, T>, "Cannot pair a name to a NameValuePair");

	// If we get passed an array, keep the type as is, otherwise store
	// a reference if we were passed an l value reference, else copy the value
	using Type = std::conditional_t<std::is_array_v<std::remove_reference_t<T>>,
			std::remove_cv_t<T>,
			std::conditional_t<std::is_lvalue_reference_v<T>,
					T,
					typename std::decay_t<T>>>;

public:
	const char* name;
	Type value;

private:
	NameValuePair& operator=(const NameValuePair&) = delete;

public:
	//! Constructs a new NameValuePair
	/*! @param n The name of the pair
		@param v The value to pair.  Ideally this should be an l-value reference so that
				 the value can be both loaded and saved to.  If you pass an r-value reference,
				 the NameValuePair will store a copy of it instead of a reference.  Thus you should
				 only pass r-values in cases where this makes sense, such as the result of some
				 size() call.
		@internal */
	constexpr inline NameValuePair(const char* n, T&& v) : name(n), value(std::forward<T>(v)) {}
};

// =================================================================================================

//! A specialization of make_nvp<> that simply forwards the value for archives who ignores nvp
/*! @relates NameValuePair
	@internal */
template <class Archive, class T> requires Archive::ignores_nvp
inline decltype(auto) make_nvp(const char*, T&& value) {
	return std::forward<T>(value);
}

//! A specialization of make_nvp<> that actually creates an nvp for archives who uses nvp
/*! @relates NameValuePair
	@internal */
template <class Archive, class T> requires (not Archive::ignores_nvp)
inline NameValuePair<T> make_nvp(const char* name, T&& value) {
	return {name, std::forward<T>(value)};
}

//! Creates a name value pair
/*! @relates NameValuePair
	@ingroup Utility */
template <class T>
inline NameValuePair<T> make_nvp(const std::string& name, T&& value) {
	return {name.c_str(), std::forward<T>(value)};
}

//! Creates a name value pair
/*! @relates NameValuePair
	@ingroup Utility */
template <class T>
inline NameValuePair<T> make_nvp(const char* name, T&& value) {
	return {name, std::forward<T>(value)};
}

// -------------------------------------------------------------------------------------------------

//! Creates a name value pair for the variable T with the same name as the variable
/*! @relates NameValuePair
	@ingroup Utility */
#define VIDE_NVP(T) ::vide::make_nvp(#T, T)

//! Convenience for creating a templated NVP
/*! For use in internal generic typing functions which have an
	Archive type declared
	@internal */
#define VIDE_NVP_(name, value) ::vide::make_nvp<Archive>(name, value)

// -------------------------------------------------------------------------------------------------

} // namespace vide
