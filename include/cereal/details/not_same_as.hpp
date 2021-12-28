// Created by Vader on 2021.12.28..

#pragma once

#include <type_traits>


namespace cereal {

// -------------------------------------------------------------------------------------------------

namespace detail {
template <class T, class U>
concept NotSameHelper = !std::is_same_v<T, U>;
}

template <class T, class U>
concept not_same_as = detail::NotSameHelper<T, U> && detail::NotSameHelper<U, T>;

// -------------------------------------------------------------------------------------------------

} // namespace cereal
