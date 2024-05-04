#pragma once

#include <type_traits>
#include <utility>


namespace vide {
// -------------------------------------------------------------------------------------------------

//! A wrapper around data that can be serialized in a binary fashion
/*! This class is used to demarcate data that can safely be serialized
	as a binary chunk of data.  Individual archives can then choose how
	best represent this during serialization.

	@internal */
template<class T>
struct BinaryData {
	//! Internally store the pointer as a void *, keeping const if created with
	//! a const pointer
	using PT = std::conditional_t<std::is_const_v<std::remove_pointer_t<std::remove_reference_t<T>>>,
		const void*,
		void*>;

	PT data; //!< pointer to beginning of data
	uint64_t size; //!< size in bytes

	BinaryData(T&& d, uint64_t s) :
		data(std::forward<T>(d)),
		size(s) { }
};

// -------------------------------------------------------------------------------------------------

//! Convenience function to create binary data for both const and non const pointers
/*! @param data Pointer to beginning of the data
	@param size The size in bytes of the data
	@relates BinaryData
	@ingroup Utility */
template<class T>
inline BinaryData<T> binary_data(T&& data, size_t size) {
	return {std::forward<T>(data), size};
}

// -------------------------------------------------------------------------------------------------
} // namespace vide
