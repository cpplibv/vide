// Created by Vader on 2022.02.14.

#pragma once

#include <concepts>
#include <meta>


namespace vide {

// -------------------------------------------------------------------------------------------------

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept Void = std::is_void_v<T>;

template <typename T>
concept NotVoid = not std::is_void_v<T>;

template <typename T>
concept Empty = std::meta::is_complete_type(^^T) && std::is_empty_v<T>;

template <typename T>
concept LValueReference = std::is_lvalue_reference_v<T>;

} // namespace vide --------------------------------------------------------------------------------
