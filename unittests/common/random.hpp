//

#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <string>


/// Random string of 3 to 30 uppercase letters
template <typename C>
[[nodiscard]] std::basic_string<C> random_basic_string(std::mt19937& gen) {
	std::basic_string<C> s(std::uniform_int_distribution<std::size_t>(3, 30)(gen), C{' '});
	for (C& c : s)
		c = static_cast<C>(std::uniform_int_distribution<int>('A', 'Z')(gen));
	return s;
}

/// Random string of N '0' / '1' characters, the input format of std::bitset
template <std::size_t N>
[[nodiscard]] std::string random_binary_string(std::mt19937& gen) {
	std::string s(N, ' ');
	for (char& c : s)
		c = static_cast<char>(std::uniform_int_distribution<int>('0', '1')(gen));
	return s;
}

/// Floating point values in [-10000, 10000], integers over their full range, strings via random_basic_string()
template <typename T>
[[nodiscard]] T random_value(std::mt19937& gen) {
	if constexpr (std::floating_point<T>) {
		return std::uniform_real_distribution<T>(-10000.0, 10000.0)(gen);
	} else if constexpr (std::integral<T> && sizeof(T) == sizeof(char)) {
		// uniform_int_distribution is undefined for char sized types (bool included)
		return static_cast<T>(std::uniform_int_distribution<std::int64_t>(
				std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen));
	} else if constexpr (std::integral<T>) {
		return std::uniform_int_distribution<T>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen);
	} else if constexpr (std::same_as<T, std::string>) {
		return random_basic_string<char>(gen);
	} else {
		static_assert(false, "random_value: unsupported type");
	}
}

/// Uniform index in [min, max], both inclusive
[[nodiscard]] inline std::size_t random_index(std::size_t min, std::size_t max, std::mt19937& gen) {
	return std::uniform_int_distribution<std::size_t>(min, max)(gen);
}
