// Created by Vader on 2022.02.14.

#pragma once

#include <vide/types/enum.hpp>

#include <concepts>


namespace vide {

// -------------------------------------------------------------------------------------------------

/// Binary serializable types:
/// - Arithmetic type except bool (bool must be saved as 0 or 1 and on read fail if any other value is read)
/// - Enum types that are unbounded (std::byte is an enum type)
template <typename T>
concept binary_serializable_type =
		(std::is_arithmetic_v<T> && !std::is_same_v<std::remove_cvref_t<T>, bool>) ||
		(std::is_enum_v<T> && enum_value_set::is_binary_serializable<T>);

} // namespace vide --------------------------------------------------------------------------------
