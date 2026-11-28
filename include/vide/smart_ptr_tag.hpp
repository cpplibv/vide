#pragma once

#include <string_view>


namespace vide { // --------------------------------------------------------------------------------

/// Identity of a smart pointer family, compared by address: a shared object loaded through one family
/// cannot be referenced again through another. The name only appears in error messages.
///
///     constexpr inline SmartPtrTag type_tag_std_shared_ptr{"std::shared_ptr"};
///     stored.assign(ptr, typeid(T), &type_tag_std_shared_ptr);
///     if (stored.pointerType != &type_tag_std_shared_ptr) ...
struct SmartPtrTag {
	std::string_view name;
};

} // namespace vide --------------------------------------------------------------------------------
