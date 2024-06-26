#pragma once

#include <type_traits>
#include <utility>

#include <vide/size_type.hpp> // Convenience include as size_type is always used with SizeTag


namespace vide {

// -------------------------------------------------------------------------------------------------

//! A wrapper around size metadata
/*! This class provides a way for archives to have more flexibility over how
	they choose to serialize size metadata for containers.  For some archive
	types, the size may be implicitly encoded in the output (e.g. JSON) and
	not need an explicit entry.  Specializing serialize or load/save for
	your archive and SizeTags allows you to choose what happens.

	@internal */
template <class T>
class SizeTag {
	// Store a reference if passed an lvalue reference, otherwise
	// make a copy of the data
	using Type = std::conditional_t<std::is_lvalue_reference_v<T>, T, std::decay_t<T>>;

public:
	Type size;
	SizeTag(Type sz) : size(std::forward<Type>(sz)) {}

private:
	SizeTag& operator=(SizeTag const&) = delete;
};

// -------------------------------------------------------------------------------------------------

//! Creates a size tag from some variable.
/*! Will normally be used to serialize size (e.g. size()) information for
variable size containers.  If you have a variable sized container,
the very first thing it serializes should be its size, wrapped in
a SizeTag.

	@relates SizeTag
	@ingroup Utility */
template <class T>
inline SizeTag<T> make_size_tag(T&& sz) {
	return {std::forward<T>(sz)};
}

// -------------------------------------------------------------------------------------------------

} // namespace vide
