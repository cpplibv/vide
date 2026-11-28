#pragma once

#include <vide/macros.hpp>

#include <cstddef>
#include <meta>
#include <string>
#include <string_view>


namespace vide { // --------------------------------------------------------------------------------
namespace detail {

/// Display name of T for vide's compile errors, highlighted when VIDE_DIAGNOSTIC_COLORS is on
template <class T>
consteval std::string diagnostic_name() {
	return VIDE_DIAGNOSTIC_HIGHLIGHT_BEGIN + std::string(std::meta::display_string_of(^^T)) + VIDE_DIAGNOSTIC_HIGHLIGHT_END;
}

/// Decimal digits of value; std::to_string is not constexpr yet
// TODO: Replace with std::to_string once constexpr: P3438 (C++26), not yet in GCC 16.2
consteval std::string diagnostic_number(std::size_t value) {
	std::string digits;
	do {
		digits.insert(digits.begin(), static_cast<char>('0' + (value % 10)));
		value /= 10;
	} while (value != 0);
	return digits;
}

/// Frames text as one of vide's compile errors; text is a few "    " indented lines, each ending in '\n'.
/// The result is static storage, as static_assert needs a message that outlives the call
consteval std::string_view diagnostic_message(std::string_view text) {
	return std::define_static_string(
			"\n"
			"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
			"\n"
			+ std::string(text) +
			"\n"
			"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}

} // namespace detail
} // namespace vide --------------------------------------------------------------------------------
