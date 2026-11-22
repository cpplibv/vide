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
#ifndef VIDE_TEST_STRUCTS_MINIMAL_H_
#define VIDE_TEST_STRUCTS_MINIMAL_H_

#include "common.hpp"


class MemberMinimal {
public:
	std::string x;

protected:
	friend class vide::access;

	template <class Archive>
	std::string save_minimal(const Archive&) const {
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, const std::string& str) {
		x = str;
	}
};

class MemberMinimalRecursive {
public:
	std::string x;

protected:
	friend class vide::access;

	template <class Archive>
	MemberMinimal save_minimal(const Archive&) const {
		return MemberMinimal{x};
	}

	template <class Archive>
	void load_minimal(const Archive&, const MemberMinimal& str) {
		x = str.x;
	}
};

class MemberMinimalRecursiveNested {
public:
	std::string x;

protected:
	friend class vide::access;

	template <class Archive>
	MemberMinimalRecursive save_minimal(const Archive&) const {
		return MemberMinimalRecursive{x};
	}

	template <class Archive>
	void load_minimal(const Archive&, const MemberMinimalRecursive& str) {
		x = str.x;
	}
};

class MemberMinimalContRef {
public:
	std::string x;

protected:
	friend class vide::access;

	template <class Archive>
	const std::string& save_minimal(const Archive&) const {
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, const std::string& str) {
		x = str;
	}
};

class MemberMinimalValueOutAndValueIn {
public:
	std::string x;

protected:
	friend class vide::access;

	template <class Archive>
	std::string save_minimal(const Archive&) const {
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, std::string str) {
		x = std::move(str);
	}
};

class MemberMinimalContRefOutAndMoveRefIn {
public:
	std::string x;

protected:
	friend class vide::access;

	template <class Archive>
	const std::string& save_minimal(const Archive&) const {
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, std::string&& str) {
		x = std::move(str);
	}
};

class MemberMinimalContRefOutAndMoveRefInVersioned {
public:
	std::string x;

protected:
	friend class vide::access;

	template <class Archive>
	const std::string& save_minimal(const Archive&, const std::uint32_t) const {
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, std::string&& str, const std::uint32_t) {
		x = std::move(str);
	}
};

class MemberMinimalVersioned {
public:
	double x = 42;

protected:
	friend class vide::access;

	template <class Archive>
	double save_minimal(const Archive&, const std::uint32_t) const {
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, const double& d, const std::uint32_t) {
		x = d;
	}
};

struct GlobalMinimal {
	std::uint32_t x = 42;
};

template <>
struct vide::serializer<GlobalMinimal> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, GlobalMinimal& var, std::uint32_t) {
		ar(var.x);
	}
};

struct GlobalMinimalVersioned {
	bool x;
};

template <>
struct vide::serializer<GlobalMinimalVersioned> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, GlobalMinimalVersioned& var, std::uint32_t version) {
		ar(var.x);
		(void) version;
	}
};

struct GlobalMinimalContRefOutAndMoveRefIn {
	std::string x;
};

template <>
struct vide::serializer<GlobalMinimalContRefOutAndMoveRefIn> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, GlobalMinimalContRefOutAndMoveRefIn& var, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar(var.x);
		} else {
			std::string str;
			ar(str);
			var.x = std::move(str);
		}
	}
};

struct GlobalMinimalContRefOutAndMoveRefInVersioned {
	std::string x;
};

template <>
struct vide::serializer<GlobalMinimalContRefOutAndMoveRefInVersioned> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, GlobalMinimalContRefOutAndMoveRefInVersioned& var, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar(var.x);
		} else {
			std::string str;
			ar(str);
			var.x = std::move(str);
		}
	}
};

// gmtgsplbv

struct TrivialGSPL {
	int x = 4;
};

template <>
struct vide::serializer<TrivialGSPL> {
	template <class Archive>
	static void serialize(Archive& ar, TrivialGSPL& var, std::uint32_t) {
		ar(var.x);
	}
};

class GlobalMinimalTrivialGSPLByValue {
public:
	TrivialGSPL x;

protected:
	friend class vide::access;
};

template <>
struct vide::serializer<GlobalMinimalTrivialGSPLByValue> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, GlobalMinimalTrivialGSPLByValue& var, std::uint32_t) {
		ar(var.x);
	}
};

// -------------------------------------------------------------------------------------------------

struct TestStruct {
	MemberMinimal mm;
	MemberMinimalRecursive mm_recursive;
	MemberMinimalRecursiveNested mm_recursive_nested;
	MemberMinimalContRef mmcr;
	MemberMinimalValueOutAndValueIn mmvv;
	MemberMinimalContRefOutAndMoveRefIn mmcrmr;
	MemberMinimalContRefOutAndMoveRefInVersioned mmcrmrv;
	MemberMinimalVersioned mmv;
	GlobalMinimal nmm;
	GlobalMinimalVersioned nmmv;
	GlobalMinimalContRefOutAndMoveRefIn gmcrmr;
	GlobalMinimalContRefOutAndMoveRefInVersioned gmcrmrv;
	GlobalMinimalTrivialGSPLByValue gmtgsplbv;

	TestStruct() = default;

	TestStruct(const std::string& s, double d, std::uint32_t u, bool b) :
			mm(s),
			mm_recursive(s),
			mm_recursive_nested(s),
			mmcr(s),
			mmvv(s),
			mmcrmr(s),
			mmcrmrv(s),
			mmv(d),
			nmm(u),
			nmmv(b),
			gmcrmr(s),
			gmcrmrv(s) {}

	template <class Archive>
	void serialize(Archive& ar) {
		ar(mm);
		ar(mm_recursive);
		ar.nvp("nest", mm_recursive_nested);
		ar(mmcr);
		ar(mmvv);
		ar(mmcrmr);
		ar(mmcrmrv);
		ar(mmv);
		ar(nmm);
		ar(nmmv);
		ar(gmcrmr);
		ar(gmcrmrv);
		ar(gmtgsplbv);
	}
};

// -------------------------------------------------------------------------------------------------

struct Issue79Struct {
	std::int32_t x;
};

// Representation depends on the archive: decimal string in text archives, raw int otherwise
template <>
struct vide::serializer<Issue79Struct> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, Issue79Struct& var, std::uint32_t) {
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

struct Issue79StructInternal {
	std::int32_t x;

public:
	template <class Archive> requires Archive::is_text_archive
	inline std::string save_minimal(const Archive&) const {
		return std::to_string(x);
	}

	template <class Archive> requires Archive::is_text_archive
	inline void load_minimal(const Archive&, const std::string& str) {
		x = std::stoi(str);
	}

	template <class Archive> requires (not Archive::is_text_archive)
	inline std::int32_t save_minimal(const Archive&) const {
		return x;
	}

	template <class Archive> requires (not Archive::is_text_archive)
	inline void load_minimal(const Archive&, std::int32_t const& xx) {
		x = xx;
	}
};

// -------------------------------------------------------------------------------------------------

struct StructMemberSerializeMinimal {
	std::int32_t x;

	template <typename Archive>	std::int32_t& serialize_minimal(Archive&) {
		return x;
	}
};

struct StructGlobalSerializeMinimal {
	std::int32_t x;
};

template <>
struct vide::serializer<StructGlobalSerializeMinimal> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, StructGlobalSerializeMinimal& var, std::uint32_t) {
		ar(var.x);
	}
};

struct StructMemberSerializeMinimalVersioned {
	std::int32_t x;

	template <typename Archive> std::int32_t& serialize_minimal(Archive&, std::uint32_t version) {
		(void) version;
		return x;
	}
};

struct StructGlobalSerializeMinimalVersioned {
	std::int32_t x;
};

template <>
struct vide::serializer<StructGlobalSerializeMinimalVersioned> {
	static constexpr bool minimal = true;

	template <class Archive>
	static void serialize(Archive& ar, StructGlobalSerializeMinimalVersioned& var, std::uint32_t version) {
		ar(var.x);
		(void) version;
	}
};

// -------------------------------------------------------------------------------------------------

template <class IArchive, class OArchive>
inline void test_structs_minimal() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		TestStruct o_struct = {random_basic_string<char>(gen), random_value<double>(gen),
				random_value<std::uint32_t>(gen), random_value<uint8_t>(gen) % 2 ? true : false};

		Issue79Struct o_struct2 = {random_value<std::int32_t>(gen)};
		Issue79StructInternal o_struct3 = {random_value<std::int32_t>(gen)};
		StructMemberSerializeMinimal o_smsm{random_value<std::int32_t>(gen)};
		StructGlobalSerializeMinimal o_sgsm{random_value<std::int32_t>(gen)};
		StructMemberSerializeMinimalVersioned o_smsmv{random_value<std::int32_t>(gen)};
		StructGlobalSerializeMinimalVersioned o_sgsmv{random_value<std::int32_t>(gen)};

		std::ostringstream os;
		{
			OArchive oar(os);
			oar(o_struct);
			oar(o_struct2);
			oar(o_struct3);
			oar(o_smsm);
			oar(o_sgsm);
			oar(o_smsmv);
			oar(o_sgsmv);
		}

		decltype(o_struct) i_struct;
		decltype(o_struct2) i_struct2;
		decltype(o_struct3) i_struct3;
		decltype(o_smsm) i_smsm;
		decltype(o_sgsm) i_sgsm;
		decltype(o_smsmv) i_smsmv;
		decltype(o_sgsmv) i_sgsmv;

		std::istringstream is(os.str());
		{
			IArchive iar(is);
			iar(i_struct);
			iar(i_struct2);
			iar(i_struct3);
			iar(i_smsm);
			iar(i_sgsm);
			iar(i_smsmv);
			iar(i_sgsmv);
		}

		CHECK_EQ(o_struct.mm.x, i_struct.mm.x);
		CHECK_EQ(o_struct.mm_recursive.x, i_struct.mm_recursive.x);
		CHECK_EQ(o_struct.mm_recursive_nested.x, i_struct.mm_recursive_nested.x);
		CHECK_EQ(o_struct.mmcr.x, i_struct.mmcr.x);
		CHECK_EQ(o_struct.mmcrmr.x, i_struct.mmcrmr.x);
		CHECK_EQ(o_struct.mmv.x, doctest::Approx(i_struct.mmv.x).epsilon(1e-5));

		CHECK_EQ(o_struct.nmm.x, i_struct.nmm.x);
		CHECK_EQ(o_struct.nmmv.x, i_struct.nmmv.x);

		CHECK_EQ(o_struct2.x, i_struct2.x);

		CHECK_EQ(o_struct3.x, i_struct3.x);

		CHECK_EQ(o_smsm.x, i_smsm.x);
		CHECK_EQ(o_sgsm.x, i_sgsm.x);
		CHECK_EQ(o_smsmv.x, i_smsmv.x);
		CHECK_EQ(o_sgsmv.x, i_sgsmv.x);
	}
}

#endif // VIDE_TEST_STRUCTS_MINIMAL_H_
