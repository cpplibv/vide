//

#include <vide/types/std_valarray.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <valarray>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


constexpr std::size_t element_count = 100;

template <typename T>
std::valarray<T> random_valarray(std::mt19937& gen) {
	const auto elements = random_container<std::vector<T>>(gen, element_count);
	return std::valarray<T>(elements.data(), elements.size());
}

/// std::valarray == compares element-wise
template <typename T>
bool same_elements(const std::valarray<T>& a, const std::valarray<T>& b) {
	return a.size() == b.size() && std::equal(std::begin(a), std::end(a), std::begin(b));
}

template <typename Ar, typename T>
void check_valarray_roundtrip(const std::valarray<T>& values) {
	const CallLog calls;
	CHECK(same_elements(roundtrip<Ar>(values), values));
	check_once_each(calls, values);
}

TEST_CASE_TEMPLATE("std::valarray", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic") {
		check_valarray_roundtrip<Ar>(random_valarray<int>(gen));
	}

	SUBCASE("structs") {
		for_each_sample_struct([&]<typename S> {
			check_valarray_roundtrip<Ar>(random_valarray<S>(gen));
		});
	}

	SUBCASE("empty") {
		check_valarray_roundtrip<Ar>(std::valarray<int>{});
	}
}
