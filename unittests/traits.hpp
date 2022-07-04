#pragma once

#include "common.hpp"


// -------------------------------------------------------------------------------------------------

// --- TestSerializeMember
struct TestSerializeMember {
	int x;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(x);
	}
};

// --- TestSerializeGlobal
struct TestSerializeGlobal {
	int x;
};

template <class Archive>
void serialize(Archive& ar, TestSerializeGlobal& var) {
	ar(var.x);
}

// --- TestSplitMember
struct TestSplitMember {
	int x;

	template <class Archive>
	void save(Archive& ar) const {
		ar(x);
	}

	template <class Archive>
	void load(Archive& ar) {
		ar(x);
	}
};

// --- TestSplitGlobal
struct TestSplitGlobal {
	int x;
};

template <class Archive>
void save(Archive& ar, const TestSplitGlobal& var) {
	ar(var.x);
}

template <class Archive>
void load(Archive& ar, TestSplitGlobal& var) {
	ar(var.x);
}

// --- TestMinimalMember
struct TestMinimalMember {
	int x;

	template <class Archive>
	int save_minimal(const Archive& ar) const {
		(void) ar;
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive& ar, const int& value) {
		(void) ar;
		x = value;
	}
};

// --- TestMinimalGlobal
struct TestMinimalGlobal {
	int x;
};

template <class Archive>
int save_minimal(const Archive& ar, const TestMinimalGlobal& var) {
	(void) ar;
	return var.x;
}

template <class Archive>
void load_minimal(const Archive& ar, TestMinimalGlobal& var, const int& value) {
	(void) ar;
	var.x = value;
}

// --- TestSerializeMemberVersioned
struct TestSerializeMemberVersioned {
	int x;

	template <class Archive>
	void serialize(Archive& ar, std::uint32_t version) {
		(void) version;
		ar(x);
	}
};

// --- TestSerializeGlobalVersioned
struct TestSerializeGlobalVersioned {
	int x;
};

template <class Archive>
void serialize(Archive& ar, TestSerializeGlobalVersioned& var, std::uint32_t version) {
	(void) version;
	ar(var.x);
}

// --- TestSplitMemberVersioned
struct TestSplitMemberVersioned {
	int x;

	template <class Archive>
	void save(Archive& ar, std::uint32_t version) const {
		(void) version;
		ar(x);
	}

	template <class Archive>
	void load(Archive& ar, std::uint32_t version) {
		(void) version;
		ar(x);
	}
};

// --- TestSplitGlobalVersioned
struct TestSplitGlobalVersioned {
	int x;
};

template <class Archive>
void save(Archive& ar, const TestSplitGlobalVersioned& var, std::uint32_t version) {
	(void) version;
	ar(var.x);
}

template <class Archive>
void load(Archive& ar, TestSplitGlobalVersioned& var, std::uint32_t version) {
	(void) version;
	ar(var.x);
}

// --- TestMinimalMemberVersioned
struct TestMinimalMemberVersioned {
	int x;

	template <class Archive>
	int save_minimal(const Archive& ar, std::uint32_t version) const {
		(void) ar;
		(void) version;
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive& ar, const int& value, std::uint32_t version) {
		(void) ar;
		(void) version;
		x = value;
	}
};

// --- TestMinimalGlobalVersioned
struct TestMinimalGlobalVersioned {
	int x;
};

template <class Archive>
int save_minimal(const Archive& ar, const TestMinimalGlobalVersioned& var, std::uint32_t version) {
	(void) ar;
	(void) version;
	return var.x;
}

template <class Archive>
void load_minimal(const Archive& ar, TestMinimalGlobalVersioned& var, const int& value, std::uint32_t version) {
	(void) ar;
	(void) version;
	var.x = value;
}

// =================================================================================================


template <class IArchive, class OArchive>
inline void test_traits() {
	{
		using I = vide::serialization_traits<IArchive, TestSerializeMember>;
		using O = vide::serialization_traits<OArchive, TestSerializeMember>;

		CHECK(I::has_serialize_member);
		CHECK(O::has_serialize_member);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestSerializeGlobal>;
		using O = vide::serialization_traits<OArchive, TestSerializeGlobal>;

		CHECK(I::has_serialize_global);
		CHECK(O::has_serialize_global);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestSplitMember>;
		using O = vide::serialization_traits<OArchive, TestSplitMember>;

		CHECK(I::has_save_member);
		CHECK(I::has_load_member);
		CHECK(O::has_save_member);
		CHECK(O::has_load_member);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestSplitGlobal>;
		using O = vide::serialization_traits<OArchive, TestSplitGlobal>;

		CHECK(I::has_save_global);
		CHECK(I::has_load_global);
		CHECK(O::has_save_global);
		CHECK(O::has_load_global);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestMinimalMember>;
		using O = vide::serialization_traits<OArchive, TestMinimalMember>;

		CHECK(I::has_save_minimal_member);
		CHECK(I::has_load_minimal_member);
		CHECK(O::has_save_minimal_member);
		CHECK(O::has_load_minimal_member);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestMinimalGlobal>;
		using O = vide::serialization_traits<OArchive, TestMinimalGlobal>;

		CHECK(I::has_save_minimal_global);
		CHECK(I::has_load_minimal_global);
		CHECK(O::has_save_minimal_global);
		CHECK(O::has_load_minimal_global);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestSerializeMemberVersioned>;
		using O = vide::serialization_traits<OArchive, TestSerializeMemberVersioned>;

		CHECK(I::has_serialize_member_versioned);
		CHECK(O::has_serialize_member_versioned);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestSerializeGlobalVersioned>;
		using O = vide::serialization_traits<OArchive, TestSerializeGlobalVersioned>;

		CHECK(I::has_serialize_global_versioned);
		CHECK(O::has_serialize_global_versioned);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestSplitMemberVersioned>;
		using O = vide::serialization_traits<OArchive, TestSplitMemberVersioned>;

		CHECK(I::has_save_member_versioned);
		CHECK(I::has_load_member_versioned);
		CHECK(O::has_save_member_versioned);
		CHECK(O::has_load_member_versioned);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestSplitGlobalVersioned>;
		using O = vide::serialization_traits<OArchive, TestSplitGlobalVersioned>;

		CHECK(I::has_save_global_versioned);
		CHECK(I::has_load_global_versioned);
		CHECK(O::has_save_global_versioned);
		CHECK(O::has_load_global_versioned);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestMinimalMemberVersioned>;
		using O = vide::serialization_traits<OArchive, TestMinimalMemberVersioned>;

		CHECK(I::has_save_minimal_member_versioned);
		CHECK(I::has_load_minimal_member_versioned);
		CHECK(O::has_save_minimal_member_versioned);
		CHECK(O::has_load_minimal_member_versioned);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
	{
		using I = vide::serialization_traits<IArchive, TestMinimalGlobalVersioned>;
		using O = vide::serialization_traits<OArchive, TestMinimalGlobalVersioned>;

		CHECK(I::has_save_minimal_global_versioned);
		CHECK(I::has_load_minimal_global_versioned);
		CHECK(O::has_save_minimal_global_versioned);
		CHECK(O::has_load_minimal_global_versioned);
		CHECK(I::is_input_serializable);
		CHECK(O::is_output_serializable);
	}
}
