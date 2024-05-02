#include <vide/vide.hpp>
#include <vide/archives/binary.hpp>
#include <vide/archives/json.hpp>
#include <vide/archives/xml.hpp>

#include <vide/archives/proxy_archive.hpp>

#include <vide/types/array.hpp>
#include <vide/types/base_class.hpp>
#include <vide/types/complex.hpp>
#include <vide/types/map.hpp>
#include <vide/types/memory.hpp>
#include <vide/types/string.hpp>
#include <vide/types/utility.hpp>
#include <vide/types/vector.hpp>
#include <vide/types/bitset.hpp>

#include <sstream>
#include <fstream>
#include <cassert>
#include <complex>
#include <iostream>
#include <iomanip>
#include <string>
#include <bitset>


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
		ar(VIDE_NVP_("b", b));
		ar(VIDE_NVP(c));
		ar(VIDE_NVP(d));
		ar(VIDE_NVP(nullptr));
		ar(nullptr);
		ar(vecA);
		ar(VIDE_NVP(vecB));
		ar(helloA);
		ar(VIDE_NVP(helloB));
		ar(bits0);
		ar(VIDE_NVP(bits1));
		ar(complex0);
		ar(VIDE_NVP(complex1));
		ar(enum0);
		ar(VIDE_NVP(enum1));
	}
};

template <typename Ar>
struct UserProxyArchive : vide::ProxyArchive<UserProxyArchive<Ar>, Ar> {
	using vide::ProxyArchive<UserProxyArchive<Ar>, Ar>::ProxyArchive;

	int my_user_data = 42;
};

// =================================================================================================

int main() {
	std::cout << std::boolalpha << std::endl;

	{
		vide::JSONOutputArchive oar(std::cout);
		UserProxyArchive<vide::JSONOutputArchive> ctxar(oar);
//		vide::XMLOutputArchive oar(std::cout);
//		UserProxyArchive<vide::XMLOutputArchive> ctxar(oar);

		TestType0 t0;
//		ctxar(t0);
		ctxar(vide::make_nvp("t1", t0));
//		oar(t0);
//		ctxar.template operator()<int>(t0.b);
	}

	return 0;
}
