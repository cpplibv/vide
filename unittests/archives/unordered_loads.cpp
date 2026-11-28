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

#include <vide/types/std_pair.hpp>
#include <vide/types/std_vector.hpp>

#include <sstream>
#include <utility>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/serialization_calls.hpp"


/// Loads its members in another order than it saves them, and x by position
struct Reordered {
	int x = 0;
	int xx = 0;
	int y = 0;
	int z = 0;

	template <class Archive>
	void save(Archive& ar) const {
		record_call<Archive>(this);
		ar.nvp("x", x);
		ar.nvp("z", z);
		ar.nvp("y", y);
		ar.nvp("xx", xx);
	}

	template <class Archive>
	void load(Archive& ar) {
		record_call<Archive>(this);
		ar(x);
		ar.nvp("y", y);
		ar.nvp("z", z);
		ar.nvp("xx", xx);
	}

	bool operator==(const Reordered&) const = default;
};

// Text archives look values up by name, so loading may follow another order than saving; an unnamed load takes the
// next value in saving order

TEST_CASE_TEMPLATE("unordered loads", Ar, VIDE_ALL_TEXT_ARCHIVES) {
	auto gen = test_rng();

	const int first = random_value<int>(gen);
	const double second = random_value<double>(gen);
	const auto third = random_container<std::vector<bool>>(gen, 5);
	const int fourth = random_value<int>(gen);
	const int fifth = random_value<int>(gen);
	const int sixth = random_value<int>(gen);
	const auto seventh = std::pair{random_value<float>(gen), Reordered{random_value<int>(gen), random_value<int>(gen),
			random_value<int>(gen), random_value<int>(gen)}};

	const CallLog calls;

	std::ostringstream os;
	{
		typename Ar::output ar(os);
		ar.nvp("first", first);
		ar.nvp("second", second);
		ar.nvp("third", third);
		ar.nvp("fourth", fourth);
		ar.nvp("fifth", fifth);
		ar.nvp("sixth", sixth);
		ar.nvp("seventh", seventh);
	}

	int loaded_first = 0;
	double loaded_second = 0;
	std::vector<bool> loaded_third;
	int loaded_fourth = 0;
	int loaded_fifth = 0;
	int loaded_sixth = 0;
	std::pair<float, Reordered> loaded_seventh;

	std::istringstream is(os.str());
	{
		typename Ar::input ar(is);
		ar.nvp("seventh", loaded_seventh);
		ar.nvp("second", loaded_second);
		ar.nvp("fourth", loaded_fourth);
		ar.nvp("third", loaded_third);
		ar.nvp("first", loaded_first);
		ar.nvp("fifth", loaded_fifth);
		ar(loaded_sixth);
	}

	CHECK_EQ(loaded_first, first);
	CHECK_EQ(loaded_second, second);
	CHECK_EQ(loaded_third, third);
	CHECK_EQ(loaded_fourth, fourth);
	CHECK_EQ(loaded_fifth, fifth);
	CHECK_EQ(loaded_sixth, sixth);
	CHECK_EQ(loaded_seventh, seventh);
	calls.check<Reordered>(1);
}
