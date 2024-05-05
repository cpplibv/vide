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

template <class Archive> inline
std::uint32_t save_minimal(const Archive&, const GlobalMinimal& nmm) {
	return nmm.x;
}

template <class Archive> inline
void load_minimal(const Archive&, GlobalMinimal& nmm, const std::uint32_t& data) {
	nmm.x = data;
}

struct GlobalMinimalVersioned {
	bool x;
};

template <class Archive> inline
bool save_minimal(const Archive&, const GlobalMinimalVersioned& nmm, const std::uint32_t) {
	return nmm.x;
}

template <class Archive> inline
void load_minimal(const Archive&, GlobalMinimalVersioned& nmm, const bool& data, const std::uint32_t) {
	nmm.x = data;
}

struct GlobalMinimalContRefOutAndMoveRefIn {
	std::string x;
};

template <class Archive>
const std::string& save_minimal(const Archive&, const GlobalMinimalContRefOutAndMoveRefIn& var) {
	return var.x;
}

template <class Archive>
void load_minimal(const Archive&, GlobalMinimalContRefOutAndMoveRefIn& var, std::string&& str) {
	var.x = std::move(str);
}

struct GlobalMinimalContRefOutAndMoveRefInVersioned {
	std::string x;
};

template <class Archive>
const std::string& save_minimal(const Archive&, const GlobalMinimalContRefOutAndMoveRefInVersioned& var, const std::uint32_t) {
	return var.x;
}

template <class Archive>
void load_minimal(const Archive&, GlobalMinimalContRefOutAndMoveRefInVersioned& var, std::string&& str, const std::uint32_t) {
	var.x = std::move(str);
}

// gmtgsplbv

struct TrivialGSPL {
	int x = 4;
};

template <class Archive>
void load(Archive& ar, TrivialGSPL& t) { ar(t.x); }

template <class Archive>
void save(Archive& ar, const TrivialGSPL& t) { ar(t.x); }

class GlobalMinimalTrivialGSPLByValue {
public:
	TrivialGSPL x;

protected:
	friend class vide::access;
};

template <class Archive>
TrivialGSPL save_minimal(const Archive&, const GlobalMinimalTrivialGSPLByValue& var) {
	return var.x;
}

template <class Archive>
void load_minimal(const Archive&, GlobalMinimalTrivialGSPLByValue& var, TrivialGSPL v) {
	var.x = std::move(v);
}

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

template <class Archive> requires Archive::is_text_archive
inline std::string save_minimal(const Archive&, Issue79Struct const& val) {
	return std::to_string(val.x);
}

template <class Archive> requires Archive::is_text_archive
inline void load_minimal(const Archive&, Issue79Struct& val, const std::string& str) {
	val.x = std::stoi(str);
}

template <class Archive> requires (not Archive::is_text_archive)
inline std::int32_t save_minimal(const Archive&, const Issue79Struct& val) {
	return val.x;
}

template <class Archive> requires (not Archive::is_text_archive)
inline void load_minimal(const Archive&, Issue79Struct& val, std::int32_t const& xx) {
	val.x = xx;
}

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

template <class IArchive, class OArchive>
inline void test_structs_minimal() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		TestStruct o_struct = {random_basic_string<char>(gen), random_value<double>(gen),
				random_value<std::uint32_t>(gen), random_value<uint8_t>(gen) % 2 ? true : false};

		Issue79Struct o_struct2 = {random_value<std::int32_t>(gen)};
		Issue79StructInternal o_struct3 = {random_value<std::int32_t>(gen)};

		std::ostringstream os;
		{
			OArchive oar(os);
			oar(o_struct);
			oar(o_struct2);
			oar(o_struct3);
		}

		decltype(o_struct) i_struct;
		decltype(o_struct2) i_struct2;
		decltype(o_struct3) i_struct3;

		std::istringstream is(os.str());
		{
			IArchive iar(is);
			iar(i_struct);
			iar(i_struct2);
			iar(i_struct3);
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
	}
}

#endif // VIDE_TEST_STRUCTS_MINIMAL_H_
