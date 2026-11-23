//

#include <vide/types/std_atomic.hpp>

#include <atomic>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/sample_structs.hpp"


/// check_roundtrip() for std::atomic<T>, which can neither be compared nor returned
template <typename Ar, typename T>
void check_atomic_roundtrip(const T& value) {
	const CallLog calls;
	const std::atomic<T> saved{value};
	std::atomic<T> loaded{};
	load_into<Ar>(save<Ar>(saved), loaded);
	CHECK_EQ(loaded.load(), value);
	check_once_each(calls, saved);
}

TEST_CASE_TEMPLATE("std::atomic", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("arithmetic") {
		check_atomic_roundtrip<Ar>(random_value<bool>(gen));
		check_atomic_roundtrip<Ar>(random_value<signed char>(gen));
		check_atomic_roundtrip<Ar>(random_value<unsigned short>(gen));
		check_atomic_roundtrip<Ar>(random_value<int>(gen));
		check_atomic_roundtrip<Ar>(random_value<long>(gen));
		check_atomic_roundtrip<Ar>(random_value<unsigned long long>(gen));
		check_atomic_roundtrip<Ar>(random_value<double>(gen));
	}

	SUBCASE("structs") {
		for_each_sample_struct([&]<typename S> {
			check_atomic_roundtrip<Ar>(random_value<S>(gen));
		});
	}
}
