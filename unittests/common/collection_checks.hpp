//

#pragma once

#include <iterator>
#include <utility>

#include <doctest.h>


// Lets CHECK_EQ print the elements of map like collections
template <typename F, typename S>
struct doctest::StringMaker<std::pair<F, S>> {
	static doctest::String convert(const std::pair<F, S>& p) {
		return "([" + doctest::toString(p.first) + "], [" + doctest::toString(p.second) + "])";
	}
};

/// Checks that both collections have the same size and equal elements in the same order
template <typename T>
void check_collection(const T& a, const T& b) {
	CHECK_EQ(std::ranges::distance(a), std::ranges::distance(b));

	for (auto a_it = std::ranges::begin(a), b_it = std::ranges::begin(b); a_it != std::ranges::end(a); ++a_it, ++b_it)
		CHECK_EQ(*a_it, *b_it);
}

/// Like check_collection(), but compares the pointees of the elements
template <typename T>
void check_ptr_collection(const T& a, const T& b) {
	CHECK_EQ(std::ranges::distance(a), std::ranges::distance(b));

	for (auto a_it = std::ranges::begin(a), b_it = std::ranges::begin(b); a_it != std::ranges::end(a); ++a_it, ++b_it)
		CHECK_EQ(**a_it, **b_it);
}
