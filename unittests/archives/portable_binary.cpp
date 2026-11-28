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

#include <vide/archives/binary.hpp>
#include <vide/archives/portable_binary.hpp>
#include <vide/details/util.hpp>

#include <bit>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include <doctest/doctest.h>

#include "common/for_each_type.hpp"
#include "common/random.hpp"


template <typename T>
T byte_swapped(T value) {
	vide::portable_binary_detail::swap_bytes<sizeof(T)>(reinterpret_cast<std::uint8_t*>(&value));
	return value;
}

/// Bit pattern, so swapped floating point values compare reliably (a swapped float can be NaN)
template <typename T>
auto bits(T value) {
	if constexpr (sizeof(T) == 1)
		return std::bit_cast<std::uint8_t>(value);
	else if constexpr (sizeof(T) == 2)
		return std::bit_cast<std::uint16_t>(value);
	else if constexpr (sizeof(T) == 4)
		return std::bit_cast<std::uint32_t>(value);
	else
		return std::bit_cast<std::uint64_t>(value);
}

using InputOptions = vide::PortableBinaryInputArchive::Options;
using OutputOptions = vide::PortableBinaryOutputArchive::Options;

constexpr int values_per_type = 100;

// The stream records the byte order it was written in; the input option chooses the byte order of the loaded values.
// Loaded values therefore come out byte swapped exactly when the input option differs from the native order

TEST_CASE("portable binary byte order options") {
	auto gen = test_rng();

	for (const bool output_little : {false, true}) {
		for (const bool input_little : {false, true}) {
			CAPTURE(output_little);
			CAPTURE(input_little);
			const auto output_options = output_little ? OutputOptions::LittleEndian() : OutputOptions::BigEndian();
			const auto input_options = input_little ? InputOptions::LittleEndian() : InputOptions::BigEndian();
			const bool swapped = vide::portable_binary_detail::is_little_endian() != input_little;

			for_each_type<bool, std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t, std::int32_t,
					std::uint64_t, std::int64_t, float, double>([&]<typename T> {
				for (int i = 0; i < values_per_type; ++i) {
					const T value = random_value<T>(gen);

					std::ostringstream os;
					{
						vide::PortableBinaryOutputArchive ar(os, output_options);
						ar(value);
					}
					T loaded{};
					std::istringstream is(os.str());
					{
						vide::PortableBinaryInputArchive ar(is, input_options);
						ar(loaded);
					}

					CHECK_EQ(bits(loaded), bits(swapped ? byte_swapped(value) : value));
				}
			});

			SUBCASE("binary_data swaps per element") { // A sized container would read a swapped size
				const auto elements = random_container<std::vector<std::int32_t>>(gen, 100);

				std::ostringstream os;
				{
					vide::PortableBinaryOutputArchive ar(os, output_options);
					ar.binary_data(elements.data(), elements.size());
				}
				std::vector<std::int32_t> loaded(elements.size());
				std::istringstream is(os.str());
				{
					vide::PortableBinaryInputArchive ar(is, input_options);
					ar.binary_data(loaded.data(), loaded.size());
				}

				for (std::size_t i = 0; i < elements.size(); ++i)
					CHECK_EQ(loaded[i], swapped ? byte_swapped(elements[i]) : elements[i]);
			}

			SUBCASE("C arrays swap per innermost element") {
				std::int32_t elements[2][3];
				for (auto& row : elements)
					for (auto& element : row)
						element = random_value<std::int32_t>(gen);

				std::ostringstream os;
				{
					vide::PortableBinaryOutputArchive ar(os, output_options);
					ar(elements);
				}
				std::int32_t loaded[2][3]{};
				std::istringstream is(os.str());
				{
					vide::PortableBinaryInputArchive ar(is, input_options);
					ar(loaded);
				}

				for (std::size_t i = 0; i < 2; ++i)
					for (std::size_t j = 0; j < 3; ++j)
						CHECK_EQ(loaded[i][j], swapped ? byte_swapped(elements[i][j]) : elements[i][j]);
			}
		}
	}
}

TEST_CASE("portable binary loads data of the foreign byte order into native order") {
	auto gen = test_rng();

	for_each_type<bool, std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t, std::int32_t,
			std::uint64_t, std::int64_t, float, double>([&]<typename T> {
		for (int i = 0; i < values_per_type; ++i) {
			const T value = random_value<T>(gen);

			// Hand made foreign data: the header claims the other byte order, the value is swapped accordingly
			std::ostringstream os;
			{
				vide::BinaryOutputArchive ar(os);
				ar(static_cast<std::uint8_t>(!vide::portable_binary_detail::is_little_endian()));
				ar(byte_swapped(value));
			}
			T loaded{};
			std::istringstream is(os.str());
			{
				vide::PortableBinaryInputArchive ar(is);
				ar(loaded);
			}

			CHECK_EQ(bits(loaded), bits(value));
		}
	});
}

namespace mynamespace {
struct MyCustomClass {};
} // namespace mynamespace

TEST_CASE("demangledName") {
#ifdef _MSC_VER
	CHECK_EQ(vide::util::demangledName<mynamespace::MyCustomClass>(), "struct mynamespace::MyCustomClass");
#else
	CHECK_EQ(vide::util::demangledName<mynamespace::MyCustomClass>(), "mynamespace::MyCustomClass");
#endif
}
