//

#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <doctest/doctest.h>


/// Generator seeded with doctest's --rand-seed (0 by default), so a failing run repeats exactly
[[nodiscard]] inline std::mt19937 test_rng() {
	return std::mt19937{doctest::getContextOptions()->rand_seed};
}


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

template <typename T>
inline constexpr bool is_pair = false;

template <typename F, typename S>
inline constexpr bool is_pair<std::pair<F, S>> = true;

/// Floating point values in [-10000, 10000], integers over their full range, strings via random_basic_string(),
/// pairs member-wise. Other types opt in with an ADL found make_random(std::type_identity<T>, std::mt19937&)
template <typename T>
[[nodiscard]] T random_value(std::mt19937& gen) {
	if constexpr (requires { make_random(std::type_identity<T>{}, gen); }) {
		return make_random(std::type_identity<T>{}, gen);
	} else if constexpr (is_pair<T>) {
		// Sequenced on purpose: the order of evaluation in a braced initializer is fixed, unlike in a call
		return T{random_value<std::remove_const_t<typename T::first_type>>(gen), random_value<typename T::second_type>(gen)};
	} else if constexpr (std::floating_point<T>) {
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

/// std::queue, std::stack and std::priority_queue: a container_type, but no iteration (unlike boost's flat
/// containers, which name their storage container_type too)
template <typename C>
concept container_adapter = requires { typename C::container_type; } && !requires(const C& c) { c.begin(); };

/// Container of up to size random elements (duplicate keys collapse): random_container<std::set<int>>(gen, 100).
/// Container adapters get a random underlying container
template <typename C>
[[nodiscard]] C random_container(std::mt19937& gen, std::size_t size) {
	if constexpr (container_adapter<C> && requires { typename C::value_compare; }) { // std::priority_queue
		return C(typename C::value_compare{}, random_container<typename C::container_type>(gen, size));
	} else if constexpr (container_adapter<C>) { // std::queue, std::stack
		return C(random_container<typename C::container_type>(gen, size));
	} else {
		std::vector<std::remove_const_t<typename C::value_type>> elements;
		for (std::size_t i = 0; i < size; ++i)
			elements.push_back(random_value<std::remove_const_t<typename C::value_type>>(gen));
		return C(elements.begin(), elements.end());
	}
}

/// Uniform index in [min, max], both inclusive
[[nodiscard]] inline std::size_t random_index(std::size_t min, std::size_t max, std::mt19937& gen) {
	return std::uniform_int_distribution<std::size_t>(min, max)(gen);
}
