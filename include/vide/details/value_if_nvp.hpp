//

#pragma once

#include <vide/nvp.hpp>


namespace vide { // ----------------------------------------------------------------------------------------------------

template <typename T>
constexpr inline const T& value_if_nvp(const NameValuePair<T>& var) noexcept {
	return var.value;
}

template <typename T>
constexpr inline const T& value_if_nvp(const T& var) noexcept {
	return var;
}

} // namespace vide ----------------------------------------------------------------------------------------------------
