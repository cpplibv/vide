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
#ifndef VIDE_TEST_VERSIONING_H_
#define VIDE_TEST_VERSIONING_H_
#include "common.hpp"

#if VIDE_THREAD_SAFE
#include <future>
#endif

namespace Nested {
struct NestedClass {
	int x;

	template<class Archive>
	void serialize(Archive& ar) { ar(x); }
};
}

VIDE_CLASS_VERSION(Nested::NestedClass, 1)

class VersionStructMS {
public:
	bool x;
	std::uint32_t v;

private:
	friend class vide::access;

	template<class Archive>
	void serialize(Archive& ar, const std::uint32_t version) {
		ar(x);
		v = version;
	}
};

struct VersionStructMSP {
	uint8_t x;
	std::uint32_t v;

	template<class Archive>
	void save(Archive& ar, const std::uint32_t /*version*/) const {
		ar(x);
	}

	template<class Archive>
	void load(Archive& ar, const std::uint32_t version) {
		ar(x);
		v = version;
	}
};

struct VersionStructNMS {
	std::int32_t x;
	std::uint32_t v;
};

template<class Archive>
void serialize(Archive& ar, VersionStructNMS& vnms, const std::uint32_t version) {
	ar(vnms.x);
	vnms.v = version;
}

struct VersionStructNMSP {
	double x;
	std::uint32_t v;
};

template<class Archive>
void save(Archive& ar, VersionStructNMSP const& vnms, const std::uint32_t /*version*/) {
	ar(vnms.x);
}

template<class Archive>
void load(Archive& ar, VersionStructNMSP& vnms, const std::uint32_t version) {
	ar(vnms.x);
	vnms.v = version;
}

VIDE_CLASS_VERSION(VersionStructMSP, 33)
VIDE_CLASS_VERSION(VersionStructNMS, 66)
VIDE_CLASS_VERSION(VersionStructNMSP, 99)

class StructStaticMemberClassVersionPub {
public:
	double x;
	std::uint32_t v;

public:
	static constexpr std::uint32_t serialize_class_version = 133;
	template<class Archive>
	void serialize(Archive& ar, const std::uint32_t version) {
		ar(x);
		v = version;
	}
};

class StructStaticMemberClassVersionPriv {
public:
	double x;
	std::uint32_t v;

private:
	friend class vide::access;

	static constexpr std::uint32_t serialize_class_version = 166;
	template<class Archive>
	void serialize(Archive& ar, const std::uint32_t version) {
		ar(x);
		v = version;
	}
};

template<class IArchive, class OArchive>
inline void test_versioning() {

	CHECK(not vide::access::has_static_member_class_version<VersionStructMS>);
	CHECK(not vide::access::has_static_member_class_version<VersionStructMSP>);
	CHECK(not vide::access::has_static_member_class_version<VersionStructNMS>);
	CHECK(not vide::access::has_static_member_class_version<VersionStructNMSP>);
	CHECK(not vide::access::has_static_member_class_version<VersionStructMS>);
	CHECK(not vide::access::has_static_member_class_version<VersionStructMSP>);
	CHECK(not vide::access::has_static_member_class_version<VersionStructNMS>);
	CHECK(not vide::access::has_static_member_class_version<VersionStructNMSP>);
	CHECK(vide::access::has_static_member_class_version<StructStaticMemberClassVersionPub>);
	CHECK_EQ(vide::access::static_member_class_version<StructStaticMemberClassVersionPub>(), 133u);
	CHECK(vide::access::has_static_member_class_version<StructStaticMemberClassVersionPriv>);
	CHECK_EQ(vide::access::static_member_class_version<StructStaticMemberClassVersionPriv>(), 166u);

	std::random_device rd;
	std::mt19937 gen(rd());

#if VIDE_THREAD_SAFE
  #include <future>
  static std::mutex testMutex;
#endif

	for (size_t i = 0; i < 100; ++i) {
		VersionStructMS o_MS = {random_value<uint8_t>(gen) % 2 ? true : false, 1};
		VersionStructMSP o_MSP = {random_value<uint8_t>(gen), 1};
		VersionStructNMS o_NMS = {random_value<int32_t>(gen), 1};
		VersionStructNMSP o_NMSP = {random_value<double>(gen), 1};
		VersionStructMS o_MS2 = {random_value<uint8_t>(gen) % 2 ? true : false, 1};
		VersionStructMSP o_MSP2 = {random_value<uint8_t>(gen), 1};
		VersionStructNMS o_NMS2 = {random_value<int32_t>(gen), 1};
		VersionStructNMSP o_NMSP2 = {random_value<double>(gen), 1};
		StructStaticMemberClassVersionPub o_SMCVPub = {random_value<double>(gen), 1};
		StructStaticMemberClassVersionPriv o_SMCVPriv = {random_value<double>(gen), 1};

		std::ostringstream os;
		{
			OArchive oar(os);
			oar(o_MS);
			oar(o_MSP);
			oar(o_NMS);
			oar(o_NMSP);
			oar(o_MS2);
			oar(o_MSP2);
			oar(o_NMS2);
			oar(o_NMSP2);
			oar(o_SMCVPub);
			oar(o_SMCVPriv);
		}

		decltype(o_MS) i_MS;
		decltype(o_MSP) i_MSP;
		decltype(o_NMS) i_NMS;
		decltype(o_NMSP) i_NMSP;
		decltype(o_MS2) i_MS2;
		decltype(o_MSP2) i_MSP2;
		decltype(o_NMS2) i_NMS2;
		decltype(o_NMSP2) i_NMSP2;
		decltype(o_SMCVPub) i_SMCVPub;
		decltype(o_SMCVPriv) i_SMCVPriv;

		std::istringstream is(os.str());
		{
			IArchive iar(is);
			iar(i_MS);
			iar(i_MSP);
			iar(i_NMS);
			iar(i_NMSP);
			iar(i_MS2);
			iar(i_MSP2);
			iar(i_NMS2);
			iar(i_NMSP2);
			iar(i_SMCVPub);
			iar(i_SMCVPriv);
		}

#if VIDE_THREAD_SAFE
    std::lock_guard<std::mutex> lock( testMutex );
#endif

		CHECK_EQ(o_MS.x, i_MS.x);
		CHECK_EQ(i_MS.v, 0u);
		CHECK_EQ(o_MSP.x, i_MSP.x);
		CHECK_EQ(i_MSP.v, 33u);
		CHECK_EQ(o_NMS.x, i_NMS.x);
		CHECK_EQ(i_NMS.v, 66u);
		CHECK_EQ(o_NMSP.x, doctest::Approx(i_NMSP.x).epsilon(1e-5));
		CHECK_EQ(i_NMSP.v, 99u);

		CHECK_EQ(o_MS2.x, i_MS2.x);
		CHECK_EQ(i_MS2.v, 0u);
		CHECK_EQ(o_MSP2.x, i_MSP2.x);
		CHECK_EQ(i_MSP2.v, 33u);
		CHECK_EQ(o_NMS2.x, i_NMS2.x);
		CHECK_EQ(i_NMS2.v, 66u);
		CHECK_EQ(o_NMSP2.x, doctest::Approx(i_NMSP2.x).epsilon(1e-5));
		CHECK_EQ(i_NMSP2.v, 99u);

		CHECK_EQ(o_SMCVPub.x, doctest::Approx(i_SMCVPub.x).epsilon(1e-5));
		CHECK_EQ(i_SMCVPub.v, 133u);
		CHECK_EQ(o_SMCVPriv.x, doctest::Approx(i_SMCVPriv.x).epsilon(1e-5));
		CHECK_EQ(i_SMCVPriv.v, 166u);
	}
}

#if VIDE_THREAD_SAFE
template <class IArchive, class OArchive> inline
void test_versioning_threading()
{
  std::vector<std::future<bool>> pool;
  for( size_t i = 0; i < 100; ++i )
    pool.emplace_back( std::async( std::launch::async,
                                   [](){ test_versioning<IArchive, OArchive>(); return true; } ) );

  for( auto & future : pool )
    future.wait();

  for( auto & future : pool )
    CHECK_UNARY( future.get() );
}
#endif // VIDE_THREAD_SAFE

#endif // VIDE_TEST_VERSIONING_H_
