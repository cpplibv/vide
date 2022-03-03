#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>

#include <cereal/archives/proxy_archive.hpp>

#include <cereal/types/array.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/bitset.hpp>

#include <sstream>
#include <fstream>
#include <cassert>
#include <complex>
#include <iostream>
#include <iomanip>
#include <string>
#include <bitset>


//// ###################################
//struct Test1 {
//	int a;
//
//private:
//	friend class cereal::access;
//
//	template <class Archive>
//	void serialize(Archive& ar) {
//		ar(CEREAL_NVP(a));
//	}
//};
//
//// ###################################
//class Test2 {
//public:
//	Test2() {}
//
//	Test2(int x) : a(x) {}
//
//	int a;
//
//private:
//	friend class cereal::access;
//
//	template <class Archive>
//	void save(Archive& ar) const {
//		ar(a);
//	}
//
//	template <class Archive>
//	void load(Archive& ar) {
//		ar(a);
//	}
//};
//
//// ###################################
//struct Test3 {
//	int a;
//};
//
//template <class Archive>
//void serialize(Archive& ar, Test3& t) {
//	ar(CEREAL_NVP(t.a));
//}
//
//namespace test4 {
//// ###################################
//struct Test4 {
//	int a;
//};
//
//template <class Archive>
//void save(Archive& ar, Test4 const& t) {
//	ar(CEREAL_NVP(t.a));
//}
//
//template <class Archive>
//void load(Archive& ar, Test4& t) {
//	ar(CEREAL_NVP(t.a));
//}
//}
//
//class Private {
//public:
//	Private() : a('z') {}
//
//private:
//	char a;
//
//	friend class cereal::access;
//
//	template <class Archive>
//	void serialize(Archive& ar) {
//		ar(a);
//	}
//};
//
//struct Everything {
//	int x;
//	int y;
//	Test1 t1;
//	Test2 t2;
//	Test3 t3;
//	test4::Test4 t4;
//	std::string s;
//
//	template <class Archive>
//	void serialize(Archive& ar) {
//		ar(CEREAL_NVP(x));
//		ar(CEREAL_NVP(y));
//		ar(CEREAL_NVP(t1));
//		ar(CEREAL_NVP(t2));
//		ar(CEREAL_NVP(t3));
//		ar(CEREAL_NVP(t4));
//		ar(CEREAL_NVP(s));
//	}
//
//	bool operator==(Everything const& o) {
//		return x == o.x &&
//				y == o.y &&
//				t1.a == o.t1.a &&
//				t2.a == o.t2.a &&
//				t3.a == o.t3.a &&
//				t4.a == o.t4.a &&
//				s == o.s;
//	}
//};
//
//
//struct SubFixture {
//	int a;
//	uint64_t b;
//	float c;
//	double d;
//	std::string s;
//
//	SubFixture() : a(3),
//			b(9999),
//			c(100.1f),
//			d(2000.9),
//			s("hello, world!") {}
//
//	template <class Archive>
//	void serialize(Archive& ar) {
//		ar(CEREAL_NVP(a));
//		ar(b);
//		ar(c);
//		ar(CEREAL_NVP(d));
//		ar(CEREAL_NVP(s));
//	}
//
//	void change() {
//		a = 4;
//		b = 4;
//		c = 4;
//		d = 4;
//		s = "4";
//	}
//};
//
//struct Fixture {
//	SubFixture f1, f2, f3;
//	int array[4];
//
//	Fixture() {
//		array[0] = 1;
//		array[1] = 2;
//		array[2] = 3;
//		array[3] = 4;
//	}
//
//	template <class Archive>
//	void save(Archive& ar) const {
//		ar(f1);
//		ar(CEREAL_NVP(f2));
//		ar(f3);
//		ar.saveBinaryValue(array, sizeof(int) * 4, "cool array man");
//	}
//
//	template <class Archive>
//	void load(Archive& ar) {
//		ar(f1);
//		ar(CEREAL_NVP(f2));
//		ar(f3);
//		ar.loadBinaryValue(array, sizeof(int) * 4);
//	}
//
//	void change() {
//		f1.change();
//		f2.change();
//		f3.change();
//	}
//};
//
//struct AAA {
//	AAA() : one(1), two(2), three({{1, 2, 3}, {4, 5, 6}, {}}) {}
//
//	int one, two;
//
//	std::vector<std::vector<int>> three;
//
//	template <class Archive>
//	void serialize(Archive& ar) {
//		ar(CEREAL_NVP(one))(CEREAL_NVP(two));
//		//ar( CEREAL_NVP(three) );
//	}
//};
//
//class Stuff {
//public:
//	Stuff() {}
//
//	void fillData() {
//		std::vector<std::complex<float>> t1{{0, -1.0f},
//				{0, -2.9932f},
//				{0, -3.5f}};
//		std::vector<std::complex<float>> t2{{1.0f, 0},
//				{2.2f, 0},
//				{3.3f, 0}};
//		data["imaginary"] = t1;
//		data["real"] = t2;
//	}
//
//private:
//	std::map<std::string, std::vector<std::complex<float>>> data;
//
//	friend class cereal::access;
//
//	template <class Archive>
//	void serialize(Archive& ar) {
//		ar(CEREAL_NVP(data));
//	}
//};
//
//struct OOJson {
//	int a;
//	int b;
//	std::pair<bool, double> c;
//	float d[3];
//
//	OOJson() = default;
//
//	OOJson(int aa, int bb, bool cc, double dd) :
//			a(aa), b(bb), c{cc, dd} {
//		d[0] = 0;
//		d[1] = 1;
//		d[2] = 2;
//	}
//
//	template <class Archive>
//	void serialize(Archive& ar) {
//		ar(CEREAL_NVP(c));
//		ar(CEREAL_NVP(a));
//		ar(b);
//		ar(CEREAL_NVP(d));
//	}
//};

// =================================================================================================

enum class EnumType {
	v0, v1, v2,
};

struct TestType0 {
	int a = 0;
	int b = 1;
	std::pair<bool, double> c{true, 3.14};
	float d[3]{4, 5, 6};

	std::vector<std::vector<int>> vecA{{1, 2, 3}, {4, 5, 6}, {}};
	std::vector<std::vector<int>> vecB{{0, 0, 0}, {1, 1, 1}, {}};

	std::string helloA = "Hello World A!";
	std::string helloB = "Hello World B!";

	std::bitset<21> bits0{42 * 1024};
	std::bitset<21> bits1{42 * 1024 + 1};

	std::complex<float> complex0{3.f, 4.f};
	std::complex<float> complex1{5.f, 6.f};

	EnumType enum0;
	EnumType enum1;

	template <class Archive>
	void serialize(Archive& ar) {
		a += ar.my_user_data;

		ar(a);
		ar(CEREAL_NVP_("b", b));
		ar(CEREAL_NVP(c));
		ar(CEREAL_NVP(d));
		ar(CEREAL_NVP(nullptr));
		ar(nullptr);
		ar(vecA);
		ar(CEREAL_NVP(vecB));
		ar(helloA);
		ar(CEREAL_NVP(helloB));
		ar(bits0);
		ar(CEREAL_NVP(bits1));
		ar(complex0);
		ar(CEREAL_NVP(complex1));
		ar(enum0);
		ar(CEREAL_NVP(enum1));
	}
};

template <typename Ar>
struct UserProxyArchive : cereal::ProxyArchive<UserProxyArchive<Ar>, Ar> {
	using cereal::ProxyArchive<UserProxyArchive<Ar>, Ar>::ProxyArchive;

	int my_user_data = 42;
};

// =================================================================================================

int main() {
	std::cout << std::boolalpha << std::endl;

	{
		cereal::JSONOutputArchive oar(std::cout);
		UserProxyArchive<cereal::JSONOutputArchive> ctxar(oar);
//		cereal::XMLOutputArchive oar(std::cout);
//		UserProxyArchive<cereal::XMLOutputArchive> ctxar(oar);

		TestType0 t0;
//		ctxar(t0);
		ctxar(cereal::make_nvp("t1", t0));
//		oar(t0);
//		ctxar.template operator()<int>(t0.b);
	}

	return 0;
}
