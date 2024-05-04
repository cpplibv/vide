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
		using T = TestSerializeMember;

		CHECK(vide::access::has_member_serialize<IArchive, T>);
		CHECK(vide::access::has_member_serialize<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestSerializeGlobal;

		CHECK(vide::access::has_global_serialize<IArchive, T>);
		CHECK(vide::access::has_global_serialize<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestSplitMember;

		CHECK(vide::access::has_member_save<IArchive, T>);
		CHECK(vide::access::has_member_load<IArchive, T>);
		CHECK(vide::access::has_member_save<OArchive, T>);
		CHECK(vide::access::has_member_load<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestSplitGlobal;

		CHECK(vide::access::has_global_save<IArchive, T>);
		CHECK(vide::access::has_global_load<IArchive, T>);
		CHECK(vide::access::has_global_save<OArchive, T>);
		CHECK(vide::access::has_global_load<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestMinimalMember;

		CHECK(vide::access::has_member_save_minimal<IArchive, T>);
		CHECK(vide::access::has_member_load_minimal<IArchive, T>);
		CHECK(vide::access::has_member_save_minimal<OArchive, T>);
		CHECK(vide::access::has_member_load_minimal<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestMinimalGlobal;

		CHECK(vide::access::has_global_save_minimal<IArchive, T>);
		CHECK(vide::access::has_global_load_minimal<IArchive, T>);
		CHECK(vide::access::has_global_save_minimal<OArchive, T>);
		CHECK(vide::access::has_global_load_minimal<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestSerializeMemberVersioned;

		CHECK(vide::access::has_member_serialize_versioned<IArchive, T>);
		CHECK(vide::access::has_member_serialize_versioned<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestSerializeGlobalVersioned;

		CHECK(vide::access::has_global_serialize_versioned<IArchive, T>);
		CHECK(vide::access::has_global_serialize_versioned<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestSplitMemberVersioned;

		CHECK(vide::access::has_member_save_versioned<IArchive, T>);
		CHECK(vide::access::has_member_load_versioned<IArchive, T>);
		CHECK(vide::access::has_member_save_versioned<OArchive, T>);
		CHECK(vide::access::has_member_load_versioned<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestSplitGlobalVersioned;

		CHECK(vide::access::has_global_save_versioned<IArchive, T>);
		CHECK(vide::access::has_global_load_versioned<IArchive, T>);
		CHECK(vide::access::has_global_save_versioned<OArchive, T>);
		CHECK(vide::access::has_global_load_versioned<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestMinimalMemberVersioned;

		CHECK(vide::access::has_member_save_minimal_versioned<IArchive, T>);
		CHECK(vide::access::has_member_load_minimal_versioned<IArchive, T>);
		CHECK(vide::access::has_member_save_minimal_versioned<OArchive, T>);
		CHECK(vide::access::has_member_load_minimal_versioned<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	} {
		using T = TestMinimalGlobalVersioned;

		CHECK(vide::access::has_global_save_minimal_versioned<IArchive, T>);
		CHECK(vide::access::has_global_load_minimal_versioned<IArchive, T>);
		CHECK(vide::access::has_global_save_minimal_versioned<OArchive, T>);
		CHECK(vide::access::has_global_load_minimal_versioned<OArchive, T>);
		CHECK(vide::access::is_input_serializable<IArchive, T>);
		CHECK(vide::access::is_output_serializable<OArchive, T>);
	}
}
