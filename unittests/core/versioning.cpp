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

#include <cstddef>
#include <cstdint>
#include <future>
#include <limits>
#include <random>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/serialization_calls.hpp"


/// Versions the serialization functions of a value received; none when not called
struct SeenVersions {
	static constexpr std::uint32_t none = std::numeric_limits<std::uint32_t>::max();

	mutable std::uint32_t saved = none;
	std::uint32_t loaded = none;
};

/// The version T's serialization functions must receive
template <typename T>
constexpr std::uint32_t expected_version = 0;

// One type per versioned shape; x is the value, seen records what the functions received

namespace Nested {
struct NestedClass {
	static constexpr std::uint32_t serialize_class_version = 1;

	int x = 0;
	SeenVersions seen;

	template <class Archive>
	void serialize(Archive& ar, std::uint32_t version) {
		record_call<Archive>(this);
		ar(x);
		(Archive::is_output ? seen.saved : seen.loaded) = version;
	}
};
} // namespace Nested

template <>
constexpr std::uint32_t expected_version<Nested::NestedClass> = 1;

/// Versioned signature without a class version: receives 0
class MemberSerializeUnversioned {
	friend class vide::access;

	template <class Archive>
	void serialize(Archive& ar, std::uint32_t version) {
		record_call<Archive>(this);
		ar(x);
		(Archive::is_output ? seen.saved : seen.loaded) = version;
	}

public:
	bool x = false;
	SeenVersions seen;
};

struct MemberSplit {
	static constexpr std::uint32_t serialize_class_version = 33;

	std::uint8_t x = 0;
	SeenVersions seen;

	template <class Archive>
	void save(Archive& ar, std::uint32_t version) const {
		record_call<Archive>(this);
		ar(x);
		seen.saved = version;
	}

	template <class Archive>
	void load(Archive& ar, std::uint32_t version) {
		record_call<Archive>(this);
		ar(x);
		seen.loaded = version;
	}
};

template <>
constexpr std::uint32_t expected_version<MemberSplit> = 33;

struct SpecializedSerialize {
	std::int32_t x = 0;
	SeenVersions seen;
};

template <>
struct vide::serializer<SpecializedSerialize> {
	static constexpr std::uint32_t serialize_class_version = 66;

	template <class Archive>
	static void serialize(Archive& ar, SpecializedSerialize& value, std::uint32_t version) {
		record_call<Archive>(&value);
		ar(value.x);
		(Archive::is_output ? value.seen.saved : value.seen.loaded) = version;
	}
};

template <>
constexpr std::uint32_t expected_version<SpecializedSerialize> = 66;

struct SpecializedSplit {
	double x = 0;
	SeenVersions seen;
};

template <>
struct vide::serializer<SpecializedSplit> {
	static constexpr std::uint32_t serialize_class_version = 99;

	template <class Archive>
	static void serialize(Archive& ar, SpecializedSplit& value, std::uint32_t version) {
		record_call<Archive>(&value);
		if constexpr (Archive::is_output) {
			ar(value.x);
			value.seen.saved = version;
		} else {
			ar(value.x);
			value.seen.loaded = version;
		}
	}
};

template <>
constexpr std::uint32_t expected_version<SpecializedSplit> = 99;

class StaticVersionPublic {
public:
	static constexpr std::uint32_t serialize_class_version = 133;

	double x = 0;
	SeenVersions seen;

	template <class Archive>
	void serialize(Archive& ar, std::uint32_t version) {
		record_call<Archive>(this);
		ar(x);
		(Archive::is_output ? seen.saved : seen.loaded) = version;
	}
};

template <>
constexpr std::uint32_t expected_version<StaticVersionPublic> = 133;

class StaticVersionPrivate {
	friend class vide::access;

	static constexpr std::uint32_t serialize_class_version = 166;

	template <class Archive>
	void serialize(Archive& ar, std::uint32_t version) {
		record_call<Archive>(this);
		ar(x);
		(Archive::is_output ? seen.saved : seen.loaded) = version;
	}

public:
	double x = 0;
	SeenVersions seen;
};

template <>
constexpr std::uint32_t expected_version<StaticVersionPrivate> = 166;

// -------------------------------------------------------------------------------------------------

template <typename T>
T random_versioned(std::mt19937& gen) {
	T value;
	value.x = random_value<decltype(value.x)>(gen);
	return value;
}

/// Two T values through one archive. The version is stored once per type, so the second value must still receive it
template <typename Ar, typename T>
void check_versioned_roundtrip(std::mt19937& gen) {
	INFO("type: ", vide::util::demangledName<T>());
	const T first = random_versioned<T>(gen);
	const T second = random_versioned<T>(gen);

	const CallLog calls;
	const auto [loaded_first, loaded_second] = roundtrip<Ar>(first, second);

	CHECK_EQ(loaded_first.x, first.x);
	CHECK_EQ(loaded_second.x, second.x);
	CHECK_EQ(first.seen.saved, expected_version<T>);
	CHECK_EQ(second.seen.saved, expected_version<T>);
	CHECK_EQ(loaded_first.seen.loaded, expected_version<T>);
	CHECK_EQ(loaded_second.seen.loaded, expected_version<T>);
	calls.check<T>(2);
}

template <typename Ar>
void check_every_versioned_shape(std::mt19937& gen) {
	check_versioned_roundtrip<Ar, Nested::NestedClass>(gen);
	check_versioned_roundtrip<Ar, MemberSerializeUnversioned>(gen);
	check_versioned_roundtrip<Ar, MemberSplit>(gen);
	check_versioned_roundtrip<Ar, SpecializedSerialize>(gen);
	check_versioned_roundtrip<Ar, SpecializedSplit>(gen);
	check_versioned_roundtrip<Ar, StaticVersionPublic>(gen);
	check_versioned_roundtrip<Ar, StaticVersionPrivate>(gen);
}

TEST_CASE("versioning traits") {
	CHECK_FALSE(vide::is_versioned_class<MemberSerializeUnversioned>);
	CHECK(vide::is_versioned_class<MemberSplit>);
	CHECK(vide::is_versioned_class<SpecializedSerialize>);
	CHECK(vide::is_versioned_class<SpecializedSplit>);
	CHECK(vide::is_versioned_class<Nested::NestedClass>);
	CHECK_FALSE(vide::access::has_static_member_class_version<SpecializedSerialize>);
	CHECK(vide::access::has_static_member_class_version<StaticVersionPublic>);
	CHECK_EQ(vide::access::static_member_class_version<StaticVersionPublic>(), 133u);
	CHECK(vide::access::has_static_member_class_version<StaticVersionPrivate>);
	CHECK_EQ(vide::access::static_member_class_version<StaticVersionPrivate>(), 166u);
}

TEST_CASE_TEMPLATE("versioning", Ar, VIDE_ALL_ARCHIVES) {
	SUBCASE("each shape receives its version") {
		auto gen = test_rng();
		check_every_versioned_shape<Ar>(gen);
	}

	SUBCASE("concurrent archives") { // The version registry is shared between threads
		constexpr unsigned thread_count = 32;
		std::vector<std::future<void>> pool;
		for (unsigned i = 0; i < thread_count; ++i)
			pool.push_back(std::async(std::launch::async, [seed = test_rng()() + i] {
				std::mt19937 gen{seed};
				check_every_versioned_shape<Ar>(gen);
			}));
		for (auto& future : pool)
			CHECK_NOTHROW(future.get());
	}
}
