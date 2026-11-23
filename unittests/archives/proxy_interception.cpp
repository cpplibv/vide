//

#include <vide/archives/binary.hpp>
#include <vide/archives/json.hpp>
#include <vide/archives/proxy_archive.hpp>
#include <vide/types/std_string.hpp>
#include <vide/types/std_vector.hpp>

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <doctest.h>


TEST_SUITE_BEGIN("proxy_interception");

namespace { // -------------------------------------------------------------------------------------

struct Password {
	std::string text;

	template <class Archive>
	void serialize(Archive& ar) {
		ar.nvp("text", text);
	}
};

struct Point {
	int x = 1;
	int y = 2;

	template <class Archive>
	void serialize(Archive& ar) {
		// Archive is the proxy type when serialized through one, its members are reachable
		if constexpr (requires { ar.scale; }) {
			int scaled = x * ar.scale;
			ar.nvp("x", scaled);
		} else {
			ar.nvp("x", x);
		}
		ar.nvp("y", y);
	}
};

// Serializer accepting only the real archives: through a proxy it is not serializable_with<Proxy, Legacy>,
// so the archive serializes it itself
struct Legacy {
	int v = 7;
};

} // namespace -------------------------------------------------------------------------------------

template <>
struct vide::serializer<Legacy> {
	static void serialize(vide::BinaryOutputArchive& ar, Legacy& var, std::uint32_t) {
		ar.nvp("v", var.v);
	}

	static void serialize(vide::BinaryInputArchive& ar, Legacy& var, std::uint32_t) {
		ar.nvp("v", var.v);
	}

	static void serialize(vide::JSONOutputArchive& ar, Legacy& var, std::uint32_t) {
		ar.nvp("v", var.v);
	}
};

namespace { // -------------------------------------------------------------------------------------

struct Login {
	std::string user = "vader";
	Password password{"hunter2"};
	std::vector<Point> points{{}, {}};
	Legacy legacy;

	template <class Archive>
	void serialize(Archive& ar) {
		ar.nvp("user", user);
		ar.nvp("password", password);
		ar.nvp("points", points);
		ar.nvp("legacy", legacy);
	}
};

/// Writes every Password as "***", at any depth
template <typename Ar>
struct RedactingProxy : vide::ProxyArchive<RedactingProxy<Ar>, Ar> {
	using Base = vide::ProxyArchive<RedactingProxy<Ar>, Ar>;
	using Base::Base;
	using Base::process_as; // The forwarding overload stays visible next to the interception

	int scale = 10;

	template <class As>
	void process_as(As& as, const Password&) {
		as.process_as(as, std::string("***"));
	}
};

/// Stores every Password reversed; ReversingProxy on the input side undoes it
template <typename Ar>
struct ReversingProxy : vide::ProxyArchive<ReversingProxy<Ar>, Ar> {
	using Base = vide::ProxyArchive<ReversingProxy<Ar>, Ar>;
	using Base::Base;
	using Base::process_as;

	template <class As>
	void process_as(As& as, const Password& password) requires Ar::is_output {
		std::string reversed(password.text.rbegin(), password.text.rend());
		as.process_as(as, reversed);
	}

	template <class As>
	void process_as(As& as, Password& password) requires Ar::is_input {
		std::string reversed;
		as.process_as(as, reversed);
		password.text.assign(reversed.rbegin(), reversed.rend());
	}
};

} // namespace -------------------------------------------------------------------------------------

TEST_CASE("output interception") {
	Login login;

	std::ostringstream direct;
	{
		vide::JSONOutputArchive ar(direct);
		ar.nvp("login", login);
	}
	std::ostringstream proxied;
	{
		vide::JSONOutputArchive ar(proxied);
		RedactingProxy<vide::JSONOutputArchive> proxy(ar);
		proxy.nvp("login", login);
	}

	CHECK_NE(direct.str().find("hunter2"), std::string::npos);
	CHECK_EQ(proxied.str().find("hunter2"), std::string::npos);
	CHECK_NE(proxied.str().find("\"password\": \"***\""), std::string::npos);
	// Interception replaces the value only: the rest of the document is untouched
	CHECK_NE(proxied.str().find("\"user\": \"vader\""), std::string::npos);
	CHECK_NE(proxied.str().find("\"v\": 7"), std::string::npos);
}

TEST_CASE("proxy type reaches nested serializers") {
	Login login;

	std::stringstream ss;
	{
		vide::BinaryOutputArchive ar(ss);
		RedactingProxy<vide::BinaryOutputArchive> proxy(ar);
		proxy(login);
	}
	Login loaded;
	{
		vide::BinaryInputArchive ar(ss);
		ar(loaded);
	}

	REQUIRE_EQ(loaded.points.size(), 2u);
	for (const auto& point : loaded.points) {
		CHECK_EQ(point.x, 10); // Point saw RedactingProxy::scale through std::vector
		CHECK_EQ(point.y, 2);
	}
	CHECK_EQ(loaded.password.text, "***");
}

TEST_CASE("fallback to the archive itself") {
	Login login;
	login.legacy.v = 42;

	std::stringstream ss;
	{
		vide::BinaryOutputArchive ar(ss);
		ReversingProxy<vide::BinaryOutputArchive> proxy(ar);
		proxy(login);
	}
	Login loaded;
	loaded.legacy.v = 0;
	{
		vide::BinaryInputArchive ar(ss);
		ReversingProxy<vide::BinaryInputArchive> proxy(ar);
		proxy(loaded);
	}

	CHECK_EQ(loaded.legacy.v, 42);
}

TEST_CASE("input interception") {
	Login login;

	std::string data;
	{
		std::ostringstream os;
		{
			vide::BinaryOutputArchive ar(os);
			ReversingProxy<vide::BinaryOutputArchive> proxy(ar);
			proxy(login);
		}
		data = std::move(os).str();
	}

	Login plain;
	{
		std::istringstream is(data);
		vide::BinaryInputArchive ar(is);
		ar(plain);
	}
	Login undone;
	{
		std::istringstream is(data);
		vide::BinaryInputArchive ar(is);
		ReversingProxy<vide::BinaryInputArchive> proxy(ar);
		proxy(undone);
	}

	CHECK_EQ(plain.password.text, "2retnuh"); // Stored reversed
	CHECK_EQ(undone.password.text, "hunter2");
	CHECK_EQ(undone.user, "vader");
}

TEST_SUITE_END();
