// Created by Vader on 2022.02.14..

#pragma once

// std
#include <concepts>


namespace vide {

// -------------------------------------------------------------------------------------------------

template <typename T>
concept arithmetic = std::integral<T> || std::floating_point<T>;

// -------------------------------------------------------------------------------------------------

} // namespace vide
