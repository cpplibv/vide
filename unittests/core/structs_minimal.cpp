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

#include <vide/types/std_string.hpp>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <utility>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/serialization_calls.hpp"


// One type per minimal shape: member save_minimal / load_minimal pairs with each return and parameter form, the
// specialized serializer with minimal = true, and serialize_minimal. All record their calls

class MemberMinimal {
	friend class vide::access;

	template <class Archive>
	std::string save_minimal(const Archive&) const {
		record_call<Archive>(this);
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, const std::string& str) {
		record_call<Archive>(this);
		x = str;
	}

public:
	std::string x;

	bool operator==(const MemberMinimal&) const = default;
};

/// Saves as another minimal type, which collapses to its string as well
class MemberMinimalRecursive {
	friend class vide::access;

	template <class Archive>
	MemberMinimal save_minimal(const Archive&) const {
		record_call<Archive>(this);
		return MemberMinimal{x};
	}

	template <class Archive>
	void load_minimal(const Archive&, const MemberMinimal& inner) {
		record_call<Archive>(this);
		x = inner.x;
	}

public:
	std::string x;

	bool operator==(const MemberMinimalRecursive&) const = default;
};

class MemberMinimalRecursiveNested {
	friend class vide::access;

	template <class Archive>
	MemberMinimalRecursive save_minimal(const Archive&) const {
		record_call<Archive>(this);
		return MemberMinimalRecursive{x};
	}

	template <class Archive>
	void load_minimal(const Archive&, const MemberMinimalRecursive& inner) {
		record_call<Archive>(this);
		x = inner.x;
	}

public:
	std::string x;

	bool operator==(const MemberMinimalRecursiveNested&) const = default;
};

class MemberMinimalConstRef {
	friend class vide::access;

	template <class Archive>
	const std::string& save_minimal(const Archive&) const {
		record_call<Archive>(this);
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, const std::string& str) {
		record_call<Archive>(this);
		x = str;
	}

public:
	std::string x;

	bool operator==(const MemberMinimalConstRef&) const = default;
};

class MemberMinimalByValue {
	friend class vide::access;

	template <class Archive>
	std::string save_minimal(const Archive&) const {
		record_call<Archive>(this);
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, std::string str) {
		record_call<Archive>(this);
		x = std::move(str);
	}

public:
	std::string x;

	bool operator==(const MemberMinimalByValue&) const = default;
};

class MemberMinimalConstRefOutRvalueIn {
	friend class vide::access;

	template <class Archive>
	const std::string& save_minimal(const Archive&) const {
		record_call<Archive>(this);
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, std::string&& str) {
		record_call<Archive>(this);
		x = std::move(str);
	}

public:
	std::string x;

	bool operator==(const MemberMinimalConstRefOutRvalueIn&) const = default;
};

class MemberMinimalConstRefOutRvalueInVersioned {
	friend class vide::access;

	template <class Archive>
	const std::string& save_minimal(const Archive&, std::uint32_t) const {
		record_call<Archive>(this);
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, std::string&& str, std::uint32_t) {
		record_call<Archive>(this);
		x = std::move(str);
	}

public:
	std::string x;

	bool operator==(const MemberMinimalConstRefOutRvalueInVersioned&) const = default;
};

class MemberMinimalVersioned {
	friend class vide::access;

	template <class Archive>
	double save_minimal(const Archive&, std::uint32_t) const {
		record_call<Archive>(this);
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, const double& d, std::uint32_t) {
		record_call<Archive>(this);
		x = d;
	}

public:
	double x = 0;

	bool operator==(const MemberMinimalVersioned&) const = default;
};

struct SpecializedMinimal {
	std::uint32_t x = 0;

	bool operator==(const SpecializedMinimal&) const = default;
};

template <>
struct vide::serializer<SpecializedMinimal> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, SpecializedMinimal& var, std::uint32_t) {
		record_call<Archive>(&var);
		ar(var.x);
	}
};

struct SpecializedMinimalSplit {
	std::string x;

	bool operator==(const SpecializedMinimalSplit&) const = default;
};

template <>
struct vide::serializer<SpecializedMinimalSplit> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, SpecializedMinimalSplit& var, std::uint32_t) {
		record_call<Archive>(&var);
		if constexpr (Archive::is_output) {
			ar(var.x);
		} else {
			std::string str;
			ar(str);
			var.x = std::move(str);
		}
	}
};

struct SpecializedMinimalVersioned {
	static constexpr std::uint32_t serialize_class_version = 3;

	bool x = false;

	bool operator==(const SpecializedMinimalVersioned&) const = default;
};

template <>
struct vide::serializer<SpecializedMinimalVersioned> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, SpecializedMinimalVersioned& var, std::uint32_t) {
		record_call<Archive>(&var);
		ar(var.x);
	}
};

/// A non-minimal type as the minimal value
struct Inner {
	int x = 0;

	bool operator==(const Inner&) const = default;
};

template <>
struct vide::serializer<Inner> {
	template <class Archive>
	static void serialize(Archive& ar, Inner& var, std::uint32_t) {
		record_call<Archive>(&var);
		ar(var.x);
	}
};

struct SpecializedMinimalOfClass {
	Inner x;

	bool operator==(const SpecializedMinimalOfClass&) const = default;
};

template <>
struct vide::serializer<SpecializedMinimalOfClass> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, SpecializedMinimalOfClass& var, std::uint32_t) {
		record_call<Archive>(&var);
		ar(var.x);
	}
};

struct MemberSerializeMinimal {
	std::int32_t x = 0;

	template <typename Archive>
	std::int32_t& serialize_minimal(Archive&) {
		record_call<Archive>(this);
		return x;
	}

	bool operator==(const MemberSerializeMinimal&) const = default;
};

struct MemberSerializeMinimalVersioned {
	std::int32_t x = 0;

	template <typename Archive>
	std::int32_t& serialize_minimal(Archive&, std::uint32_t) {
		record_call<Archive>(this);
		return x;
	}

	bool operator==(const MemberSerializeMinimalVersioned&) const = default;
};

/// Representation depends on the archive: decimal string in text archives, raw int otherwise (upstream issue 79)
struct ArchiveDependentSpecialized {
	std::int32_t x = 0;

	bool operator==(const ArchiveDependentSpecialized&) const = default;
};

template <>
struct vide::serializer<ArchiveDependentSpecialized> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, ArchiveDependentSpecialized& var, std::uint32_t) {
		record_call<Archive>(&var);
		if constexpr (Archive::is_text_archive) {
			if constexpr (Archive::is_output) {
				ar(std::to_string(var.x));
			} else {
				std::string str;
				ar(str);
				var.x = std::stoi(str);
			}
		} else {
			ar(var.x);
		}
	}
};

struct ArchiveDependentMember {
	std::int32_t x = 0;

	template <class Archive> requires Archive::is_text_archive
	std::string save_minimal(const Archive&) const {
		record_call<Archive>(this);
		return std::to_string(x);
	}

	template <class Archive> requires Archive::is_text_archive
	void load_minimal(const Archive&, const std::string& str) {
		record_call<Archive>(this);
		x = std::stoi(str);
	}

	template <class Archive> requires (!Archive::is_text_archive)
	std::int32_t save_minimal(const Archive&) const {
		record_call<Archive>(this);
		return x;
	}

	template <class Archive> requires (!Archive::is_text_archive)
	void load_minimal(const Archive&, const std::int32_t& value) {
		record_call<Archive>(this);
		x = value;
	}

	bool operator==(const ArchiveDependentMember&) const = default;
};

/// Every shape as a member, one of them named
struct AllMinimal {
	MemberMinimal a;
	MemberMinimalRecursive b;
	MemberMinimalRecursiveNested c;
	MemberMinimalConstRef d;
	MemberMinimalByValue e;
	MemberMinimalConstRefOutRvalueIn f;
	MemberMinimalConstRefOutRvalueInVersioned g;
	MemberMinimalVersioned h;
	SpecializedMinimal i;
	SpecializedMinimalSplit j;
	SpecializedMinimalVersioned k;
	SpecializedMinimalOfClass l;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(a)(b);
		ar.nvp("nested", c);
		ar(d)(e)(f)(g)(h)(i)(j)(k)(l);
	}

	bool operator==(const AllMinimal&) const = default;
};

std::string without_whitespace(std::string text) {
	std::erase_if(text, [](unsigned char c) { return std::isspace(c); });
	return text;
}

// -------------------------------------------------------------------------------------------------

TEST_CASE_TEMPLATE("minimal serialization", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();
	const std::string text = random_value<std::string>(gen);
	const auto number = random_value<std::int32_t>(gen);

	SUBCASE("member save_minimal / load_minimal") {
		check_roundtrip<Ar, MemberMinimal>(MemberMinimal{text});
		check_roundtrip<Ar, MemberMinimalRecursive>(MemberMinimalRecursive{text});
		check_roundtrip<Ar, MemberMinimalRecursiveNested>(MemberMinimalRecursiveNested{text});
		check_roundtrip<Ar, MemberMinimalConstRef>(MemberMinimalConstRef{text});
		check_roundtrip<Ar, MemberMinimalByValue>(MemberMinimalByValue{text});
		check_roundtrip<Ar, MemberMinimalConstRefOutRvalueIn>(MemberMinimalConstRefOutRvalueIn{text});
		check_roundtrip<Ar, MemberMinimalConstRefOutRvalueInVersioned>(MemberMinimalConstRefOutRvalueInVersioned{text});
		check_roundtrip<Ar, MemberMinimalVersioned>(MemberMinimalVersioned{random_value<double>(gen)});
	}

	SUBCASE("specialized serializer with minimal = true") {
		check_roundtrip<Ar, SpecializedMinimal>(SpecializedMinimal{random_value<std::uint32_t>(gen)});
		check_roundtrip<Ar, SpecializedMinimalSplit>(SpecializedMinimalSplit{text});
		check_roundtrip<Ar, SpecializedMinimalVersioned>(SpecializedMinimalVersioned{true});
		check_roundtrip<Ar, SpecializedMinimalOfClass, Inner>(SpecializedMinimalOfClass{Inner{number}});
	}

	SUBCASE("member serialize_minimal") {
		check_roundtrip<Ar, MemberSerializeMinimal>(MemberSerializeMinimal{number});
		check_roundtrip<Ar, MemberSerializeMinimalVersioned>(MemberSerializeMinimalVersioned{number});
	}

	SUBCASE("representation chosen by archive") {
		check_roundtrip<Ar, ArchiveDependentSpecialized>(ArchiveDependentSpecialized{number});
		check_roundtrip<Ar, ArchiveDependentMember>(ArchiveDependentMember{number});
	}

	SUBCASE("as members") {
		check_roundtrip<Ar>(AllMinimal{{text}, {text}, {text}, {text}, {text}, {text}, {text}, {1.5}, {7}, {text}, {true}, {{number}}});
	}
}

TEST_CASE("minimal types are written as a bare value") {
	CHECK_EQ(without_whitespace(save<JSONArchives>(MemberMinimal{"abc"})), R"({"value0":"abc"})");
	CHECK_EQ(without_whitespace(save<JSONArchives>(MemberMinimalRecursiveNested{"abc"})), R"({"value0":"abc"})");
	CHECK_EQ(without_whitespace(save<JSONArchives>(SpecializedMinimal{42})), R"({"value0":42})");
	CHECK_EQ(without_whitespace(save<JSONArchives>(MemberSerializeMinimal{-7})), R"({"value0":-7})");
	CHECK_EQ(without_whitespace(save<JSONArchives>(ArchiveDependentSpecialized{12})), R"({"value0":"12"})");
	CHECK_EQ(without_whitespace(save<JSONArchives>(ArchiveDependentMember{12})), R"({"value0":"12"})");
}
