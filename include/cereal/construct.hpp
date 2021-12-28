#pragma once

#include <type_traits>
#include <iostream>
#include <cstdint>
#include <functional>

#include <cereal/access.hpp>
#include <cereal/construct_fwd.hpp>
#include <cereal/details/helpers.hpp>
#include <cereal/macros.hpp>
#include <cereal/specialize.hpp>


namespace cereal {

// =================================================================================================

//! A class that allows cereal to load smart pointers to types that have no default constructor
/*! If your class does not have a default constructor, cereal will not be able
	to load any smart pointers to it unless you overload LoadAndConstruct
	for your class, and provide an appropriate load_and_construct method.  You can also
	choose to define a member static function instead of specializing this class.

	The specialization of LoadAndConstruct must be placed within the cereal namespace:

	@code{.cpp}
	struct MyType
	{
	  MyType( int x ); // note: no default ctor
	  int myX;

	  // Define a serialize or load/save pair as you normally would
	  template <class Archive>
	  void serialize( Archive & ar )
	  {
		ar( myX );
	  }
	};

	// Provide a specialization for LoadAndConstruct for your type
	namespace cereal
	{
	  template <> struct LoadAndConstruct<MyType>
	  {
		// load_and_construct will be passed the archive that you will be loading
		// from as well as a construct object which you can use as if it were the
		// constructor for your type.  cereal will handle all memory management for you.
		template <class Archive>
		static void load_and_construct( Archive & ar, cereal::construct<MyType> & construct )
		{
		  int x;
		  ar( x );
		  construct( x );
		}

		// if you require versioning, simply add a const std::uint32_t as the final parameter, e.g.:
		// load_and_construct( Archive & ar, cereal::construct<MyType> & construct, std::uint32_t const version )
	  };
	} // end namespace cereal
	@endcode

	Please note that just as in using external serialization functions, you cannot get
	access to non-public members of your class by befriending cereal::access.  If you
	have the ability to modify the class you wish to serialize, it is recommended that you
	use member serialize functions and a static member load_and_construct function.

	load_and_construct functions, regardless of whether they are static members of your class or
	whether you create one in the LoadAndConstruct specialization, have the following signature:

	@code{.cpp}
	// generally Archive will be templated, but it can be specific if desired
	template <class Archive>
	static void load_and_construct( Archive & ar, cereal::construct<MyType> & construct );
	// with an optional last parameter specifying the version: const std::uint32_t version
	@endcode

	Versioning behaves the same way as it does for standard serialization functions.

	@tparam T The type to specialize for
	@ingroup Access */
template <class T>
struct LoadAndConstruct {};

// forward decl for construct
//! @cond PRIVATE_NEVERDEFINED
namespace memory_detail { template <class Ar, class T> struct LoadAndConstructLoadWrapper; }
namespace boost_variant_detail { template <class Ar, class T> struct LoadAndConstructLoadWrapper; }
//! @endcond

//! Used to construct types with no default constructor
/*! When serializing a type that has no default constructor, cereal
	will attempt to call either the class static function load_and_construct
	or the appropriate template specialization of LoadAndConstruct.  cereal
	will pass that function a reference to the archive as well as a reference
	to a construct object which should be used to perform the allocation once
	data has been appropriately loaded.

	@code{.cpp}
	struct MyType
	{
	  // note the lack of default constructor
	  MyType( int xx, int yy );

	  int x, y;
	  double notInConstructor;

	  template <class Archive>
	  void serialize( Archive & ar )
	  {
		ar( x, y );
		ar( notInConstructor );
	  }

	  template <class Archive>
	  static void load_and_construct( Archive & ar, cereal::construct<MyType> & construct )
	  {
		int x, y;
		ar( x, y );

		// use construct object to initialize with loaded data
		construct( x, y );

		// access to member variables and functions via -> operator
		ar( construct->notInConstructor );

		// could also do the above section by:
		double z;
		ar( z );
		construct->notInConstructor = z;
	  }
	};
	@endcode

	@tparam T The class type being serialized
	*/
template <class T>
class construct {
public:
	//! Construct and initialize the type T with the given arguments
	/*! This will forward all arguments to the underlying type T,
		calling an appropriate constructor.

		Calling this function more than once will result in an exception
		being thrown.

		@param args The arguments to the constructor for T
		@throw Exception If called more than once */
	template <class ... Args>
	void operator()(Args&& ... args);
	// implementation deferred due to reliance on cereal::access

	//! Get a reference to the initialized underlying object
	/*! This must be called after the object has been initialized.

		@return A reference to the initialized object
		@throw Exception If called before initialization */
	T* operator->() {
		if (!itsValid)
			throw Exception("Object must be initialized prior to accessing members");

		return itsPtr;
	}

	//! Returns a raw pointer to the initialized underlying object
	/*! This is mainly intended for use with passing an instance of
		a constructed object to cereal::base_class.

		It is strongly recommended to avoid using this function in
		any other circumstance.

		@return A raw pointer to the initialized type */
	T* ptr() {
		return operator->();
	}

private:
	template <class Ar, class TT> friend struct ::cereal::memory_detail::LoadAndConstructLoadWrapper;
	template <class Ar, class TT> friend struct ::cereal::boost_variant_detail::LoadAndConstructLoadWrapper;

	construct(T* p) : itsPtr(p), itsEnableSharedRestoreFunction([]() {}), itsValid(false) {}

	construct(T* p, std::function<void()> enableSharedFunc) : // g++4.7 ice with default lambda to std func
			itsPtr(p), itsEnableSharedRestoreFunction(enableSharedFunc), itsValid(false) {}

	construct(construct const&) = delete;
	construct& operator=(construct const&) = delete;

	T* itsPtr;
	std::function<void()> itsEnableSharedRestoreFunction;
	bool itsValid;
};

// =================================================================================================

// ######################################################################
// Deferred Implementation, see construct for more information
template <class T> template <class ... Args> inline
void construct<T>::operator()(Args&& ... args) {
	if (itsValid)
		throw Exception("Attempting to construct an already initialized object");

	::cereal::access::construct(itsPtr, std::forward<Args>(args)...);
	itsEnableSharedRestoreFunction();
	itsValid = true;
}

// -------------------------------------------------------------------------------------------------

} // namespace cereal
