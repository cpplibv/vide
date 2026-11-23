/*
  Copyright (c) 2013-2022, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstdint>
#include <limits>
#include <random>
#include <string>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/for_each_type.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"


constexpr int values_per_type = 100;

TEST_CASE_TEMPLATE("arithmetic types", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();

	SUBCASE("random values") {
		for_each_type<bool, char, signed char, unsigned char, short, unsigned short, int, unsigned int, long,
				unsigned long, long long, unsigned long long, float, double>([&]<typename T> {
			for (int i = 0; i < values_per_type; ++i)
				check_roundtrip<Ar>(random_value<T>(gen));
		});
	}

	SUBCASE("limits") {
		for_each_type<signed char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long,
				long long, unsigned long long, float, double>([&]<typename T> {
			check_roundtrip<Ar>(std::numeric_limits<T>::lowest());
			check_roundtrip<Ar>(std::numeric_limits<T>::max());
			check_roundtrip<Ar>(T{0});
		});
	}

	SUBCASE("every char value") {
		for (int c = std::numeric_limits<char>::lowest(); c <= std::numeric_limits<char>::max(); ++c)
			check_roundtrip<Ar>(static_cast<char>(c));
	}

	SUBCASE("long double") { // Text archives store it with double precision
		for (int i = 0; i < values_per_type; ++i) {
			const auto value = random_value<long double>(gen);
			CHECK_EQ(roundtrip<Ar>(value), doctest::Approx(value).epsilon(1e-5));
		}
	}
}

/// A Narrow value saved as Narrow and as Wide gives the same text, which loads back as Wide
template <typename Ar, typename Narrow, typename Wide>
void check_same_text(std::mt19937& gen) {
	const Narrow narrow = random_value<Narrow>(gen);
	const Wide wide = narrow;
	CHECK_EQ(save<Ar>(narrow), save<Ar>(wide));
	CHECK_EQ(load<Ar, Wide>(save<Ar>(narrow)), wide);
}

TEST_CASE_TEMPLATE("text archives write a value the same in any integer width", Ar, VIDE_ALL_TEXT_ARCHIVES) {
	auto gen = test_rng();

	for (int i = 0; i < values_per_type; ++i) {
		for_each_type<std::int16_t, std::int32_t, std::int64_t, short, int, long, long long>([&]<typename Wide> {
			check_same_text<Ar, std::int8_t, Wide>(gen);
		});
		for_each_type<std::uint16_t, std::uint32_t, std::uint64_t, unsigned short, unsigned int, unsigned long,
				unsigned long long>([&]<typename Wide> {
			check_same_text<Ar, std::uint8_t, Wide>(gen);
		});
	}
}
