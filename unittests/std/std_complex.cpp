//

#include <vide/types/std_complex.hpp>

#include <complex>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"


template <typename T>
std::complex<T> random_complex(std::mt19937& gen) {
	const T real = random_value<T>(gen);
	return {real, random_value<T>(gen)};
}

TEST_CASE_TEMPLATE("std::complex", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	check_roundtrip<Ar>(random_complex<float>(gen));
	check_roundtrip<Ar>(random_complex<double>(gen));

	// Text archives store long double with double precision
	const auto value = random_complex<long double>(gen);
	const auto loaded = roundtrip<Ar>(value);
	CHECK_EQ(loaded.real(), doctest::Approx(value.real()).epsilon(1e-5));
	CHECK_EQ(loaded.imag(), doctest::Approx(value.imag()).epsilon(1e-5));
}
