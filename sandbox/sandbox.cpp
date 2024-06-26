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

#include <vide/vide.hpp>
#include <vide/archives/binary.hpp>
#include <vide/archives/portable_binary.hpp>
#include <vide/archives/xml.hpp>

#include <vide/types/array.hpp>
#include <vide/types/base_class.hpp>
#include <vide/types/bitset.hpp>
#include <vide/types/chrono.hpp>
#include <vide/types/complex.hpp>
#include <vide/types/map.hpp>
#include <vide/types/memory.hpp>
#include <vide/types/polymorphic.hpp>
#include <vide/types/string.hpp>
#include <vide/types/utility.hpp>
#include <vide/types/vector.hpp>

//#include <cxxabi.h>
#include <sstream>
#include <fstream>
#include <cassert>
#include <complex>
#include <iostream>
#include <random>

class Base {
private:
	friend class vide::access;

	template<class Archive>
	void serialize(Archive& ar) {
		std::cout << "Base serialize" << std::endl;
		ar(x);
	}

	virtual void foo() = 0;

public:
	int x;

	virtual ~Base() {
	}
};

class Derived : public Base {
public:
	using Base::x;

	Derived() : Base(), y() {
	}

	Derived(int d, int b) {
		y = d;
		x = b;
	}

	virtual ~Derived() {
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar(vide::virtual_base_class <Base>(this));
		std::cout << "Derived serialize" << std::endl;
		ar(y);
	}

	// Changing serialization function type is no longer allowed in inheritance hierarchies
	// But I can see that this could be a valid usecase. Maybe I will reconsider
	// template <class Archive>
	// void save( Archive & ar ) const
	// {
	//   ar( vide::virtual_base_class<Base>(this) );
	//   std::cout << "Derived save" << std::endl;
	//   ar( y );
	// }
	//
	// template <class Archive>
	// void load( Archive & ar )
	// {
	//   ar( vide::virtual_base_class<Base>(this) );
	//   std::cout << "Derived load" << std::endl;
	//   ar( y );
	// }

	void foo() {
	}

	int y;
};

//namespace vide
//{
//  template <class Archive> struct specialize<Archive, Derived, vide::specialization::member_load_save> {};
//}

VIDE_REGISTER_TYPE(Derived)

// ###################################
struct Test1 {
	int a;

private:
	friend class vide::access;

	template<class Archive>
	void serialize(Archive& ar) {
		ar(VIDE_NVP(a));
	}
};

// ###################################
class Test2 {
public:
	Test2() {
	}

	Test2(int x) : a(x) {
	}

	int a;

private:
	friend class vide::access;

	template<class Archive>
	void save(Archive& ar) const {
		ar(a);
	}

	template<class Archive>
	void load(Archive& ar) {
		ar(a);
	}
};

// ###################################
struct Test3 {
	int a;
};

template<class Archive>
void serialize(Archive& ar, Test3& t) {
	ar(VIDE_NVP(t.a));
}

namespace test4 {
// ###################################
struct Test4 {
	int a;
};

template<class Archive>
void save(Archive& ar, Test4 const& t) {
	ar(VIDE_NVP(t.a));
}

template<class Archive>
void load(Archive& ar, Test4& t) {
	ar(VIDE_NVP(t.a));
}
}

class Private {
public:
	Private() : a('z') {
	}

private:
	char a;

	friend class vide::access;

	template<class Archive>
	void serialize(Archive& ar) {
		ar(a);
	}
};

struct Everything {
	int x;
	int y;
	Test1 t1;
	Test2 t2;
	Test3 t3;
	test4::Test4 t4;
	std::chrono::system_clock::time_point time_point;
	std::chrono::system_clock::duration time_dur;
	std::string s;

	template<class Archive>
	void serialize(Archive& ar) {
		ar(VIDE_NVP(x));
		ar(VIDE_NVP(y));
		ar(VIDE_NVP(t1));
		ar(VIDE_NVP(t2));
		ar(VIDE_NVP(t3));
		ar(VIDE_NVP(t4));
		ar(VIDE_NVP(time_point));
		ar(VIDE_NVP(time_dur));
		ar(VIDE_NVP(s));
	}

	bool operator==(const Everything& o) const {
		return
				x == o.x &&
				y == o.y &&
				t1.a == o.t1.a &&
				t2.a == o.t2.a &&
				t3.a == o.t3.a &&
				t4.a == o.t4.a &&
				time_point == o.time_point &&
				time_dur == o.time_dur &&
				s == o.s;
	}
};

struct EmptyStruct {
	template<class Archive>
	void serialize(Archive&) {
		std::cout << "Side effects!" << std::endl;
	}
};

struct NonEmptyStruct {
	int x, y, z;
};

struct NoDefaultCtor {
private:
	NoDefaultCtor() {
	};
	int z;

	NoDefaultCtor(int x, bool) : y(x) {
	}

public:
	NoDefaultCtor(int x) : y(x) {
	}

	friend class vide::access;

	int y;

	template<class Archive>
	void serialize(Archive& ar) {
		ar(y);
	}
};

//namespace vide
//{
//  template <>
//  struct LoadAndConstruct<NoDefaultCtor>
//  {
//    template <class Archive>
//    static void load_and_construct( Archive & ar, vide::construct<NoDefaultCtor> & construct )
//    {
//      int y;
//      ar( y );
//      construct( y );
//    }
//  };
//}

struct unordered_naming {
	int x;
	int y;
	int z;

	template<class Archive>
	void save(Archive& ar) const {
		ar(VIDE_NVP(x),
			VIDE_NVP(z),
			VIDE_NVP(y));
	}

	template<class Archive>
	void load(Archive& ar) {
		ar(x,
			VIDE_NVP(y),
			VIDE_NVP(z));
	}

	bool operator==(unordered_naming const& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
};

std::ostream& operator<<(std::ostream& os, unordered_naming const& s) {
	os << "[x: " << s.x << " y: " << s.y << " z: " << s.z << "]";
	return os;
}

template<class IArchive, class OArchive>
void test_unordered_loads() {
	std::random_device rd;
	std::mt19937 gen(rd());

	auto rngI = []() { return 1; };
	auto rngF = []() { return 2.0f; };
	auto rngD = []() { return 3.2; };

	for (int i = 0; i < 100; ++i) {
		const auto name1 = "1";
		const auto name2 = "2";
		const auto name3 = "3";
		const auto name4 = "4";
		const auto name5 = "5";
		const auto name6 = "6";
		const auto name7 = "7";

		int o_int1 = rngI();
		double o_double2 = rngD();
		std::vector<bool> o_vecbool3 = {true, false, true, false, true};
		int o_int4 = rngI();
		int o_int5 = rngI();
		int o_int6 = rngI();
		std::pair <float, unordered_naming> o_un7;
		o_un7.first = rngF();
		o_un7.second.x = rngI();
		o_un7.second.y = rngI();
		o_un7.second.z = rngI(); {
			std::ofstream os("test.xml");
			OArchive oar(os);

			oar(vide::make_nvp(name1, o_int1),
				vide::make_nvp(name2, o_double2),
				vide::make_nvp(name3, o_vecbool3),
				vide::make_nvp(name4, o_int4),
				vide::make_nvp(name5, o_int5),
				vide::make_nvp(name6, o_int6),
				vide::make_nvp(name7, o_un7));
		}

		decltype(o_int1) i_int1;
		decltype(o_double2) i_double2;
		decltype(o_vecbool3) i_vecbool3;
		decltype(o_int4) i_int4;
		decltype(o_int5) i_int5;
		decltype(o_int6) i_int6;
		decltype(o_un7) i_un7;

		std::ifstream is("test.xml"); {
			IArchive iar(is);

			iar(vide::make_nvp(name7, o_un7),
				vide::make_nvp(name2, i_double2),
				vide::make_nvp(name4, i_int4),
				vide::make_nvp(name3, i_vecbool3),
				vide::make_nvp(name1, i_int1),
				vide::make_nvp(name5, i_int5),
				i_int6,
				i_un7);
		}
	}
}

class BoostTransitionMS {
public:
	BoostTransitionMS() {
	}

	BoostTransitionMS(int xx) : x(xx) {
	}

	int getX() { return x; }
	void setX(int xx) { x = xx; }

private:
	friend class vide::access;
	int x;

	template<class Archive>
	void serialize(Archive& ar, const std::uint32_t /*version*/) { ar(x); }
};

class BoostTransitionSplit {
public:
	BoostTransitionSplit() {
	}

	BoostTransitionSplit(int xx) : x(xx) {
	}

	int getX() { return x; }
	void setX(int xx) { x = xx; }

private:
	friend class vide::access;
	int x;

	template<class Archive>
	void save(Archive& ar, const std::uint32_t /*version*/) const { ar(x); }

	template<class Archive>
	void load(Archive& ar, const std::uint32_t /*version*/) { ar(x); }
};

class BoostTransitionNMS {
public:
	BoostTransitionNMS() {
	}

	BoostTransitionNMS(int xx) : x(xx) {
	}

	int x;
};

template<class Archive>
void serialize(Archive& ar, BoostTransitionNMS& bnms, const std::uint32_t version) {
	ar(bnms.x);
	std::cout << "NMS version: " << version << std::endl;
}

struct BoostTransitionNMSplit {
public:
	BoostTransitionNMSplit() {
	}

	BoostTransitionNMSplit(int xx) : x(xx) {
	}

	int x;
};

template<class Archive>
void save(Archive& ar, BoostTransitionNMSplit const& bnsplit, const std::uint32_t version) {
	ar(bnsplit.x);
	std::cout << "NMsave version: " << version << std::endl;
}

template<class Archive>
void load(Archive& ar, BoostTransitionNMSplit& bnsplit, const std::uint32_t version) {
	ar(bnsplit.x);
	std::cout << "NMload version: " << version << std::endl;
}

// ######################################################################
int main() {
	std::cout << std::boolalpha << std::endl;

	Everything e_out;
	e_out.x = 99;
	e_out.y = 100;
	e_out.t1 = {1};
	e_out.t2 = {2};
	e_out.t3 = {3};
	e_out.t4 = {4};
	e_out.time_point = std::chrono::system_clock::now();
	e_out.time_dur = std::chrono::seconds{3};
	e_out.s = "Hello, World!";
	std::unique_ptr <NoDefaultCtor> nodefault(new NoDefaultCtor(3));

	Test2 t2 = {22}; {
		std::ofstream os("out.txt", std::ios::binary);
		vide::BinaryOutputArchive archive(os);
		archive(VIDE_NVP(e_out));
		archive(t2);
		archive(nodefault);
	}

	Everything e_in;

	std::unique_ptr <NoDefaultCtor> nodefaultin(new NoDefaultCtor(1)); {
		std::ifstream is("out.txt", std::ios::binary);
		vide::BinaryInputArchive archive(is);
		archive(VIDE_NVP(e_in));
		archive(t2);
		archive(nodefaultin);
		std::remove("out.txt");
	}

	assert(e_in == e_out);
	assert(nodefault->y == nodefaultin->y); {
		vide::BinaryOutputArchive archive(std::cout);
		int xxx[] = {-1, 95, 3};
		archive(xxx);

		vide::XMLOutputArchive archive2(std::cout, vide::XMLOutputArchive::Options(std::numeric_limits <double>::max_digits10, true, true));
		archive2(xxx);

		std::vector<int> yyy = {1, 2, 3};
		archive2(yyy);

		archive2.saveBinaryValue(xxx, sizeof(int) * 3);
	} {
		std::ofstream os("out.xml");
		vide::XMLOutputArchive oar(os);
		//vide::XMLOutputArchive oar( std::cout );

		oar(vide::make_nvp("hello", 5));

		std::string bla("bla");
		oar(bla);

		auto intptr = std::make_shared <int>(99);
		oar(VIDE_NVP(intptr));

		std::map <std::string, int> map1 =
		{
			{"one", 1},
			{"two", 2},
			{"three", 3}
		};

		oar(VIDE_NVP(map1));

		int x = 3;
		oar(VIDE_NVP(x));
		oar(5);
		oar(3.3);
		oar(3.2f);
		oar(true);

		std::array<int, 5> arr = {{1, 2, 3, 4, 5}};
		oar(arr);

		std::vector<std::string> vec = {
			"hey",
			"there",
			"buddy"
		};

		std::vector<std::vector<std::string>> vec2 = {vec, vec, vec};

		oar(vide::make_nvp("EVERYTHING", e_out));
		oar(vec);
		oar(vec2);

		int xxx[] = {-1, 95, 3};
		oar.saveBinaryValue(xxx, sizeof(int) * 3, "xxxbinary");
		//oar.saveBinaryValue( xxx, sizeof(int)*3 );

		std::unique_ptr <Derived> d1(new Derived(3, 4));
		std::unique_ptr <Base> d2(new Derived(4, 5));
		std::shared_ptr <Base> d3(new Derived(5, 6));
		oar(d1);
		oar(d2);
		oar(d3);
	} {
		std::ifstream is("out.xml");
		vide::XMLInputArchive iar(is);

		int hello;
		iar(vide::make_nvp("hello", hello));
		assert(hello == 5);

		std::string bla;
		iar(bla);
		assert(bla == "bla");

		std::shared_ptr <int> intptr;
		iar(VIDE_NVP(intptr));
		assert(*intptr == 99);

		std::map <std::string, int> map1;

		iar(VIDE_NVP(map1));
		assert(map1["one"] == 1);
		assert(map1["two"] == 2);
		assert(map1["three"] == 3);


		int x;
		iar(VIDE_NVP(x));
		assert(x == 3);

		int x5;
		iar(x5);
		assert(x5 == 5);

		double x33;
		iar(x33);
		assert(x33 == 3.3);

		float x32;
		iar(x32);
		assert(x32 == 3.2f);

		bool xtrue;
		iar(xtrue);
		assert(xtrue == true);

		std::array<int, 5> arr;
		iar(arr);
		for (int i = 0; i < 5; ++i)
			assert(arr[i] == (i+1));

		Everything e;
		iar(vide::make_nvp("EVERYTHING", e));
		assert(e == e_out);

		std::vector<std::string> vec;
		iar(vec);
		assert(vec[0] == "hey");
		assert(vec[1] == "there");
		assert(vec[2] == "buddy");

		std::vector<std::vector<std::string>> vec2;
		iar(vec2);
		for (auto& v : vec2) {
			assert(v[0] == "hey");
			assert(v[1] == "there");
			assert(v[2] == "buddy");
		}

		int xxx[3];
		iar.loadBinaryValue(xxx, sizeof(int) * 3);
		assert(xxx[0] == -1);
		assert(xxx[1] == 95);
		assert(xxx[2] == 3);

		std::unique_ptr <Derived> d1;
		std::unique_ptr <Base> d2;
		std::shared_ptr <Base> d3;

		iar(d1);
		assert(d1->x == 4 && d1->y == 3);
		iar(d2);
		assert(dynamic_cast<Derived*>(d2.get())->x == 5 && dynamic_cast<Derived*>(d2.get())->y == 4);
		iar(d3);
		assert(dynamic_cast<Derived*>(d3.get())->x == 6 && dynamic_cast<Derived*>(d3.get())->y == 5);
	} {
		std::ofstream b("endian.out", std::ios::binary);
		vide::PortableBinaryOutputArchive oar(b);

		bool bb = true;
		char a = 'a';
		int x = 1234;
		float y = 1.324f;
		double z = 3.1452;
		long double d = 1.123451234512345;
		long long j = 2394873298472343;

		oar(bb)(a)(x)(y)(z)(d)(j);
		std::cout << bb << " " << a << " " << x << " " << y << " " << z << " " << d << " " << j << std::endl;
		// valgrind will complain about uninitialized bytes here - seems to be the padding caused by the long double and
		// long long allocations (this padding just exists on the stack and is never used anywhere)
		// see https://bugs.kde.org/show_bug.cgi?id=197915
	} {
		std::ifstream b("endian.out", std::ios::binary);
		vide::PortableBinaryInputArchive iar(b);

		bool bb;
		char a;
		int x;
		float y;
		double z;
		long double d;
		long long j;

		iar(bb)(a)(x)(y)(z)(d)(j);

		std::cout << bb << " " << a << " " << x << " " << y << " " << z << " " << d << " " << j << std::endl;

		std::remove("endian.out");
	} {
		std::ofstream ss("xml_ordering.out");
		vide::XMLOutputArchive ar(ss);

		double one = 1;
		double two = 2;
		double three = 3;
		std::vector<int> four = {1, 2, 3, 4};

		// Output is ordered 3 2 1 4
		ar(three)(VIDE_NVP(two))(one)(vide::make_nvp("five", four));
	} {
		std::ifstream ss("xml_ordering.out");
		vide::XMLInputArchive ar(ss);

		// Output out of order, try to load in order 1 2 3 4
		double one;
		double two;
		double three;
		std::vector<int> four;

		ar(one); // vide can only give warnings if you used an NVP!
		ar(VIDE_NVP(two));
		ar(three);

		try {
			ar(VIDE_NVP(three));
		} catch (vide::Exception const& e) {
			std::cout << e.what() << std::endl;
			std::cout << "Looked for three but we didn't use an NVP when saving" << std::endl;
		}
		ar(vide::make_nvp("five", four));
		ar(vide::make_nvp("five", four)); // do it a second time since it shouldn't matter as we provide the name

		std::cout << one << std::endl;
		std::cout << two << std::endl;
		std::cout << three << std::endl;
		for (auto i : four) std::cout << i << " ";
		std::cout << std::endl;
	} {
		// Boost transition layer stuff
		std::ofstream ss("vide_version.out");
		vide::XMLOutputArchive ar(ss);

		BoostTransitionMS b(3);
		ar(b)(b);

		BoostTransitionSplit c(4);
		ar(c)(c);

		BoostTransitionNMS d(5);
		ar(d)(d);

		BoostTransitionNMSplit e(32);
		ar(e)(e);
	} {
		// Boost transition layer stuff
		std::ifstream ss("vide_version.out");
		vide::XMLInputArchive ar(ss);

		BoostTransitionMS b;
		ar(b);
		assert(b.getX() == 3);
		b.setX(0);
		ar(b);
		assert(b.getX() == 3);

		BoostTransitionSplit c;
		ar(c);
		assert(c.getX() == 4);
		c.setX(0);
		ar(c);
		assert(c.getX() == 4);

		BoostTransitionNMS d;
		ar(d);
		assert(d.x == 5);
		d.x = 0;
		ar(d);
		assert(d.x == 5);

		BoostTransitionNMSplit e;
		ar(e);
		assert(e.x == 32);
		e.x = 0;
		ar(e);
		assert(e.x == 32);
	}

	return 0;
}

VIDE_CLASS_VERSION(BoostTransitionMS, 1)

VIDE_CLASS_VERSION(BoostTransitionSplit, 2)

VIDE_CLASS_VERSION(BoostTransitionNMS, 3)

// keep the other at default version (0)
