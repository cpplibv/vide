//

#include <vide/types/std_atomic.hpp>
#include <vide/types/std_chrono.hpp>
#include <vide/types/std_vector.hpp>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/serialization_calls.hpp"


// One type per serialization shape; each records its calls, so a round trip shows the detected shape is the one
// that runs

struct TestSerializeMember {
	int x = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(x);
	}

	bool operator==(const TestSerializeMember&) const = default;
};

struct TestSerializeMemberVersioned {
	int x = 0;

	template <class Archive>
	void serialize(Archive& ar, std::uint32_t) {
		record_call<Archive>(this);
		ar(x);
	}

	bool operator==(const TestSerializeMemberVersioned&) const = default;
};

struct TestSerializeMemberClassVersion {
	static constexpr std::uint32_t serialize_class_version = 3;

	int x = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(x);
	}

	bool operator==(const TestSerializeMemberClassVersion&) const = default;
};

struct TestSplitMember {
	int x = 0;

	template <class Archive>
	void save(Archive& ar) const {
		record_call<Archive>(this);
		ar(x);
	}

	template <class Archive>
	void load(Archive& ar) {
		record_call<Archive>(this);
		ar(x);
	}

	bool operator==(const TestSplitMember&) const = default;
};

struct TestSplitMemberVersioned {
	int x = 0;

	template <class Archive>
	void save(Archive& ar, std::uint32_t) const {
		record_call<Archive>(this);
		ar(x);
	}

	template <class Archive>
	void load(Archive& ar, std::uint32_t) {
		record_call<Archive>(this);
		ar(x);
	}

	bool operator==(const TestSplitMemberVersioned&) const = default;
};

struct TestMinimalMember {
	int x = 0;

	template <class Archive>
	int save_minimal(const Archive&) const {
		record_call<Archive>(this);
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, const int& value) {
		record_call<Archive>(this);
		x = value;
	}

	bool operator==(const TestMinimalMember&) const = default;
};

struct TestMinimalMemberVersioned {
	int x = 0;

	template <class Archive>
	int save_minimal(const Archive&, std::uint32_t) const {
		record_call<Archive>(this);
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, const int& value, std::uint32_t) {
		record_call<Archive>(this);
		x = value;
	}

	bool operator==(const TestMinimalMemberVersioned&) const = default;
};

struct TestSerializeMinimalMember {
	int x = 0;

	template <class Archive>
	int& serialize_minimal(Archive&) {
		record_call<Archive>(this);
		return x;
	}

	bool operator==(const TestSerializeMinimalMember&) const = default;
};

struct TestSerializeMinimalMemberVersioned {
	int x = 0;

	template <class Archive>
	int& serialize_minimal(Archive&, std::uint32_t) {
		record_call<Archive>(this);
		return x;
	}

	bool operator==(const TestSerializeMinimalMemberVersioned&) const = default;
};

/// serialize and save for output: two shapes, which the archive rejects
struct TestAmbiguousMember {
	int x = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(x);
	}

	template <class Archive>
	void save(Archive& ar) const {
		ar(x);
	}
};

struct TestSerializeSpecialized {
	int x = 0;

	bool operator==(const TestSerializeSpecialized&) const = default;
};

template <>
struct vide::serializer<TestSerializeSpecialized> {
	template <class Archive>
	static void serialize(Archive& ar, TestSerializeSpecialized& var, std::uint32_t) {
		record_call<Archive>(&var);
		ar(var.x);
	}
};

struct TestSerializeSpecializedVersioned {
	int x = 0;

	bool operator==(const TestSerializeSpecializedVersioned&) const = default;
};

template <>
struct vide::serializer<TestSerializeSpecializedVersioned> {
	static constexpr std::uint32_t serialize_class_version = 5;

	template <class Archive>
	static void serialize(Archive& ar, TestSerializeSpecializedVersioned& var, std::uint32_t) {
		record_call<Archive>(&var);
		ar(var.x);
	}
};

struct TestMinimalSpecialized {
	int x = 0;

	bool operator==(const TestMinimalSpecialized&) const = default;
};

template <>
struct vide::serializer<TestMinimalSpecialized> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, TestMinimalSpecialized& var, std::uint32_t) {
		record_call<Archive>(&var);
		ar(var.x);
	}
};

struct TestUnserializable {
	int x = 0;
};

// =================================================================================================

template <class Archive, class T>
void check_member_shape(bool minimal) {
	CHECK_EQ(vide::access::count_member_shapes<Archive, T>, 1);
	CHECK(vide::serializable_with<Archive, T>);
	CHECK_EQ(vide::minimal_with<Archive, T>, minimal);
}

TEST_CASE_TEMPLATE("serialization traits", Ar, VIDE_ALL_ARCHIVES) {
	using IArchive = typename Ar::input;
	using OArchive = typename Ar::output;

	SUBCASE("TestSerializeMember") {
		using T = TestSerializeMember;
		CHECK(vide::access::has_member_serialize<IArchive, T>);
		CHECK(vide::access::has_member_serialize<OArchive, T>);
		CHECK_FALSE(vide::access::has_member_serialize_versioned<OArchive, T>);
		check_member_shape<IArchive, T>(false);
		check_member_shape<OArchive, T>(false);
		CHECK_FALSE(vide::is_versioned_class<T>);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestSerializeMemberVersioned") {
		using T = TestSerializeMemberVersioned;
		CHECK(vide::access::has_member_serialize_versioned<IArchive, T>);
		CHECK(vide::access::has_member_serialize_versioned<OArchive, T>);
		check_member_shape<IArchive, T>(false);
		check_member_shape<OArchive, T>(false);
		CHECK_FALSE(vide::is_versioned_class<T>);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestSplitMember") {
		using T = TestSplitMember;
		CHECK(vide::access::has_member_save<OArchive, T>);
		CHECK(vide::access::has_member_load<IArchive, T>);
		check_member_shape<IArchive, T>(false);
		check_member_shape<OArchive, T>(false);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestSplitMemberVersioned") {
		using T = TestSplitMemberVersioned;
		CHECK(vide::access::has_member_save_versioned<OArchive, T>);
		CHECK(vide::access::has_member_load_versioned<IArchive, T>);
		check_member_shape<IArchive, T>(false);
		check_member_shape<OArchive, T>(false);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestMinimalMember") {
		using T = TestMinimalMember;
		CHECK(vide::access::has_member_save_minimal<OArchive, T>);
		CHECK(vide::access::has_member_load_minimal<IArchive, T>);
		check_member_shape<IArchive, T>(true);
		check_member_shape<OArchive, T>(true);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestMinimalMemberVersioned") {
		using T = TestMinimalMemberVersioned;
		CHECK(vide::access::has_member_save_minimal_versioned<OArchive, T>);
		CHECK(vide::access::has_member_load_minimal_versioned<IArchive, T>);
		check_member_shape<IArchive, T>(true);
		check_member_shape<OArchive, T>(true);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestSerializeMinimalMember") {
		using T = TestSerializeMinimalMember;
		CHECK(vide::access::has_member_serialize_minimal<OArchive, T>);
		check_member_shape<IArchive, T>(true);
		check_member_shape<OArchive, T>(true);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestSerializeMinimalMemberVersioned") {
		using T = TestSerializeMinimalMemberVersioned;
		CHECK(vide::access::has_member_serialize_minimal_versioned<OArchive, T>);
		check_member_shape<IArchive, T>(true);
		check_member_shape<OArchive, T>(true);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestAmbiguousMember") {
		using T = TestAmbiguousMember;
		CHECK_EQ(vide::access::count_member_shapes<OArchive, T>, 2);
		CHECK_EQ(vide::access::count_member_shapes<IArchive, T>, 1);
	}

	SUBCASE("TestSerializeMemberClassVersion") {
		using T = TestSerializeMemberClassVersion;
		CHECK(vide::serializable_with<IArchive, T>);
		CHECK(vide::serializable_with<OArchive, T>);
		CHECK(vide::is_versioned_class<T>);
		CHECK_EQ(vide::serializer<T>::serialize_class_version, 3u);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestSerializeSpecialized") {
		using T = TestSerializeSpecialized;
		CHECK_EQ(vide::access::count_member_shapes<OArchive, T>, 0);
		CHECK(vide::serializable_with<IArchive, T>);
		CHECK(vide::serializable_with<OArchive, T>);
		CHECK_FALSE(vide::is_versioned_class<T>);
		CHECK_FALSE(vide::minimal_with<OArchive, T>);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestSerializeSpecializedVersioned") {
		using T = TestSerializeSpecializedVersioned;
		CHECK(vide::serializable_with<IArchive, T>);
		CHECK(vide::serializable_with<OArchive, T>);
		CHECK(vide::is_versioned_class<T>);
		CHECK_EQ(vide::serializer<T>::serialize_class_version, 5u);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestMinimalSpecialized") {
		using T = TestMinimalSpecialized;
		CHECK(vide::serializable_with<IArchive, T>);
		CHECK(vide::serializable_with<OArchive, T>);
		CHECK(vide::minimal_with<IArchive, T>);
		CHECK(vide::minimal_with<OArchive, T>);
		check_roundtrip<Ar, T>(T{7});
	}

	SUBCASE("TestUnserializable") {
		using T = TestUnserializable;
		CHECK_FALSE(vide::serializable_with<IArchive, T>);
		CHECK_FALSE(vide::serializable_with<OArchive, T>);
	}

	SUBCASE("std types") {
		CHECK(vide::minimal_with<OArchive, std::chrono::seconds>);
		CHECK(vide::minimal_with<OArchive, std::atomic<int>>);
		CHECK_FALSE(vide::minimal_with<OArchive, std::vector<int>>);
	}
}
