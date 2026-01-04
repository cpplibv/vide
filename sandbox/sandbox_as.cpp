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
	v0,
	v1,
	v2,
	serialize_max_value = v2,
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

	EnumType enum0 = EnumType::v0;
	EnumType enum1 = EnumType::v1;
	int ignored = 0;
	int directLoad = 1;
	int directLoadNamed = 2;

	[[nodiscard]] constexpr inline bool operator==(const TestType0&) const = default;

	template <class Archive>
	void serialize(Archive& ar) {
		a += ar.my_user_data;

		ar(a);
		ar(VIDE_NVP_("b", b));
		ar.nvp("c", c);
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

		if constexpr (Archive::is_output) {
			ar.nvp("ignored", ignored); // we only save it here, so we can ignore it during load
			ar(directLoad);
			ar.nvp("directLoadNamed", directLoadNamed);
		} else {
			ignored = 1000;
			ar.template nvp_ignore<int>("ignored");
			directLoad = ar.template load<int>();
			directLoadNamed = ar.template nvp_load<int>("directLoadNamed");
		}
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

	TestType0 t0;
	t0.a += 1;
	t0.b += 1;
	t0.c.first = false;
	t0.c.second += 1.0;
	t0.vecA.emplace_back(std::vector<int>{0, 0});
	t0.vecB.emplace_back(std::vector<int>{});
	t0.helloA.pop_back();
	t0.helloB.push_back('!');
	t0.bits0.set(0, false);
	t0.bits1.set(0, true);
	t0.complex0 = {1.f, 2.f};
	t0.complex1 = {2.f, 3.f};
	t0.enum0 = EnumType::v2;
	t0.enum1 = EnumType::v2;
	t0.ignored += 1000;
	t0.directLoad += 1000;
	t0.directLoadNamed += 1000;

	TestType0 t1;

	{
		std::ostringstream os;

		{
			vide::JSONOutputArchive oar(os);
			UserProxyArchive<vide::JSONOutputArchive> ctxar(oar);
			ctxar(vide::make_nvp("t0", t0));
		}
		const auto data = std::move(os).str();
		std::cout << data << std::endl;

		{
			std::istringstream is{data};
			vide::JSONInputArchive iar(is);
			UserProxyArchive<vide::JSONInputArchive> ctxar(iar);
			ctxar(vide::make_nvp("t0", t0));
		}
	}

	return t0 == t1;
}
