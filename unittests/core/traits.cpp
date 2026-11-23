#include <vide/types/std_atomic.hpp>
#include <vide/types/std_chrono.hpp>
#include <vide/types/std_vector.hpp>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <vector>

#include <doctest.h>

#include "common/archive_test_cases.hpp"


// -------------------------------------------------------------------------------------------------

struct TestSerializeMember {
	int x;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(x);
	}
};

struct TestSerializeMemberVersioned {
	int x;

	template <class Archive>
	void serialize(Archive& ar, std::uint32_t version) {
		ar(x);
		(void) version;
	}
};

struct TestSerializeMemberClassVersion {
	static constexpr std::uint32_t serialize_class_version = 3;

	int x;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(x);
	}
};

struct TestSplitMember {
	int x;

	template <class Archive>
	void save(Archive& ar) const { ar(x); }

	template <class Archive>
	void load(Archive& ar) { ar(x); }
};

struct TestSplitMemberVersioned {
	int x;

	template <class Archive>
	void save(Archive& ar, std::uint32_t) const { ar(x); }

	template <class Archive>
	void load(Archive& ar, std::uint32_t) { ar(x); }
};

struct TestMinimalMember {
	int x;

	template <class Archive>
	int save_minimal(const Archive&) const { return x; }

	template <class Archive>
	void load_minimal(const Archive&, const int& value) { x = value; }
};

struct TestMinimalMemberVersioned {
	int x;

	template <class Archive>
	int save_minimal(const Archive&, std::uint32_t) const { return x; }

	template <class Archive>
	void load_minimal(const Archive&, const int& value, std::uint32_t) { x = value; }
};

struct TestSerializeMinimalMember {
	int x;

	template <class Archive>
	int& serialize_minimal(Archive&) { return x; }
};

struct TestSerializeMinimalMemberVersioned {
	int x;

	template <class Archive>
	int& serialize_minimal(Archive&, std::uint32_t) { return x; }
};

struct TestAmbiguousMember {
	int x;

	template <class Archive>
	void serialize(Archive& ar) { ar(x); }

	template <class Archive>
	void save(Archive& ar) const { ar(x); }
};

struct TestSerializeSpecialized {
	int x;
};

template <>
struct vide::serializer<TestSerializeSpecialized> {
	template <class Archive>
	static void serialize(Archive& ar, TestSerializeSpecialized& var, std::uint32_t) {
		ar(var.x);
	}
};

struct TestSerializeSpecializedVersioned {
	int x;
};

template <>
struct vide::serializer<TestSerializeSpecializedVersioned> {
	static constexpr std::uint32_t serialize_class_version = 5;

	template <class Archive>
	static void serialize(Archive& ar, TestSerializeSpecializedVersioned& var, std::uint32_t) {
		ar(var.x);
	}
};

struct TestMinimalSpecialized {
	int x;
};

template <>
struct vide::serializer<TestMinimalSpecialized> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, TestMinimalSpecialized& var, std::uint32_t) {
		ar(var.x);
	}
};

struct TestUnserializable {
	int x;
};

// =================================================================================================

template <class Archive, class T>
inline void check_member_shape(bool minimal) {
	CHECK_EQ(vide::access::count_member_shapes<Archive, T>, 1);
	CHECK(vide::serializable_with<Archive, T>);
	CHECK_EQ(vide::minimal_with<Archive, T>, minimal);
}

template <class IArchive, class OArchive>
inline void test_traits() {
	{
		using T = TestSerializeMember;

		CHECK(vide::access::has_member_serialize<IArchive, T>);
		CHECK(vide::access::has_member_serialize<OArchive, T>);
		CHECK(not vide::access::has_member_serialize_versioned<OArchive, T>);
		check_member_shape<IArchive, T>(false);
		check_member_shape<OArchive, T>(false);
		CHECK(not vide::is_versioned_class<T>);
	} {
		using T = TestSerializeMemberVersioned;

		CHECK(vide::access::has_member_serialize_versioned<IArchive, T>);
		CHECK(vide::access::has_member_serialize_versioned<OArchive, T>);
		check_member_shape<IArchive, T>(false);
		check_member_shape<OArchive, T>(false);
		CHECK(not vide::is_versioned_class<T>);
	} {
		using T = TestSplitMember;

		CHECK(vide::access::has_member_save<OArchive, T>);
		CHECK(vide::access::has_member_load<IArchive, T>);
		check_member_shape<IArchive, T>(false);
		check_member_shape<OArchive, T>(false);
	} {
		using T = TestSplitMemberVersioned;

		CHECK(vide::access::has_member_save_versioned<OArchive, T>);
		CHECK(vide::access::has_member_load_versioned<IArchive, T>);
		check_member_shape<IArchive, T>(false);
		check_member_shape<OArchive, T>(false);
	} {
		using T = TestMinimalMember;

		CHECK(vide::access::has_member_save_minimal<OArchive, T>);
		CHECK(vide::access::has_member_load_minimal<IArchive, T>);
		check_member_shape<IArchive, T>(true);
		check_member_shape<OArchive, T>(true);
	} {
		using T = TestMinimalMemberVersioned;

		CHECK(vide::access::has_member_save_minimal_versioned<OArchive, T>);
		CHECK(vide::access::has_member_load_minimal_versioned<IArchive, T>);
		check_member_shape<IArchive, T>(true);
		check_member_shape<OArchive, T>(true);
	} {
		using T = TestSerializeMinimalMember;

		CHECK(vide::access::has_member_serialize_minimal<OArchive, T>);
		check_member_shape<IArchive, T>(true);
		check_member_shape<OArchive, T>(true);
	} {
		using T = TestSerializeMinimalMemberVersioned;

		CHECK(vide::access::has_member_serialize_minimal_versioned<OArchive, T>);
		check_member_shape<IArchive, T>(true);
		check_member_shape<OArchive, T>(true);
	} {
		using T = TestAmbiguousMember;

		CHECK_EQ(vide::access::count_member_shapes<OArchive, T>, 2);
		CHECK_EQ(vide::access::count_member_shapes<IArchive, T>, 1);
	} {
		using T = TestSerializeMemberClassVersion;

		CHECK(vide::serializable_with<IArchive, T>);
		CHECK(vide::serializable_with<OArchive, T>);
		CHECK(vide::is_versioned_class<T>);
		CHECK_EQ(vide::serializer<T>::serialize_class_version, 3u);
	} {
		using T = TestSerializeSpecialized;

		CHECK_EQ(vide::access::count_member_shapes<OArchive, T>, 0);
		CHECK(vide::serializable_with<IArchive, T>);
		CHECK(vide::serializable_with<OArchive, T>);
		CHECK(not vide::is_versioned_class<T>);
		CHECK(not vide::minimal_with<OArchive, T>);
	} {
		using T = TestSerializeSpecializedVersioned;

		CHECK(vide::serializable_with<IArchive, T>);
		CHECK(vide::serializable_with<OArchive, T>);
		CHECK(vide::is_versioned_class<T>);
		CHECK_EQ(vide::serializer<T>::serialize_class_version, 5u);
	} {
		using T = TestMinimalSpecialized;

		CHECK(vide::serializable_with<IArchive, T>);
		CHECK(vide::serializable_with<OArchive, T>);
		CHECK(vide::minimal_with<IArchive, T>);
		CHECK(vide::minimal_with<OArchive, T>);
	} {
		using T = TestUnserializable;

		CHECK(not vide::serializable_with<IArchive, T>);
		CHECK(not vide::serializable_with<OArchive, T>);
	} {
		CHECK(vide::minimal_with<OArchive, std::chrono::seconds>);
		CHECK(vide::minimal_with<OArchive, std::atomic<int>>);
		CHECK(not vide::minimal_with<OArchive, std::vector<int>>);
	}
}


TEST_SUITE_BEGIN("traits");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("traits", test_traits)

TEST_SUITE_END();
