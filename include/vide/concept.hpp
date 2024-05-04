// Created by Vader on 2022.02.14..

#pragma once

// std
#include <concepts>


namespace vide {

// -------------------------------------------------------------------------------------------------

template <typename T>
concept arithmetic = std::integral<T> || std::floating_point<T>;

template <typename T>
concept binary_serializable_type = std::is_arithmetic_v<T> || std::is_same_v<T, std::byte>;

template <typename T>
concept Void = std::is_void_v<T>;

template <typename T>
concept NotVoid = not std::is_void_v<T>;

// -------------------------------------------------------------------------------------------------

} // namespace vide
