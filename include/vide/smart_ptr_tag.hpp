#pragma once

#include <string_view>


namespace vide {

// -------------------------------------------------------------------------------------------------

/// @usage
/// constexpr inline SmartPtrTag type_tag_std_shared_ptr{"std::shared_ptr"};
///
/// stored.assignFrom(ptr, ptr, typeid(T), &type_tag_std_shared_ptr);
/// stored.pointerType != &type_tag_std_shared_ptr
struct SmartPtrTag {
	std::string_view name;
};

// -------------------------------------------------------------------------------------------------

} // namespace vide
