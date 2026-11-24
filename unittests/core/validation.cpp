//

#include <vide/archives/proxy_archive.hpp>
#include <vide/types/memory.hpp>
#include <vide/types/std_optional.hpp>
#include <vide/types/std_vector.hpp>

#include <initializer_list>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/roundtrip.hpp"


template <typename Ar>
struct NonValidatingProxy : vide::ProxyArchive<NonValidatingProxy<Ar>, Ar> {
	Ar ar;
	static constexpr bool enforce_validation = false;

	template <typename Stream>
	explicit NonValidatingProxy(Stream& stream) :
		vide::ProxyArchive<NonValidatingProxy<Ar>, Ar>(ar),
		ar(stream) {}
};

struct is_odd {
	template <typename T>
	static void operator()(const T& var) {
		if (var % 2 == 0)
			throw vide::Exception("Is-odd validation failed during serialization: object is even.");
	}
};

/// Saving and loading value both pass the validators
template <typename Ar, typename T>
bool accepts(const T& value, const auto&... validators) {
	return saves<Ar>(value, validators...) && loads<Ar>(value, validators...);
}

/// Saving and loading value both fail the validators
template <typename Ar, typename T>
bool rejects(const T& value, const auto&... validators) {
	return !saves<Ar>(value, validators...) && !loads<Ar>(value, validators...);
}

/// saves() / loads() with the value written as ar.nvp("var", value, validators...)
template <typename Ar, typename T>
bool saves_nvp(const T& value, const auto&... validators) {
	try {
		std::ostringstream os;
		typename Ar::output ar(os);
		ar.nvp("var", value, validators...);
		return true;
	} catch (const vide::Exception&) {
		return false;
	}
}

template <typename Ar, typename T>
bool loads_nvp(const T& value, const auto&... validators) {
	std::ostringstream os;
	{
		typename Ar::output ar(os);
		ar.nvp("var", value);
	}

	try {
		T loaded{};
		std::istringstream is(os.str());
		typename Ar::input ar(is);
		ar.nvp("var", loaded, validators...);
		return true;
	} catch (const vide::Exception&) {
		return false;
	}
}

template <typename Ar, typename T>
bool accepts_nvp(const T& value, const auto&... validators) {
	return saves_nvp<Ar>(value, validators...) && loads_nvp<Ar>(value, validators...);
}

template <typename Ar, typename T>
bool rejects_nvp(const T& value, const auto&... validators) {
	return !saves_nvp<Ar>(value, validators...) && !loads_nvp<Ar>(value, validators...);
}

/// Reaches every validator through the archive's dependent names, so they must exist on both directions
struct DependentNameValidated {
	std::unique_ptr<int> pointer = std::make_unique<int>(1);
	std::vector<int> values{1, 2};
	std::optional<std::vector<int>> optional{std::in_place, std::initializer_list<int>{1}};
	std::vector<std::vector<int>> nested{{1}, {2, 3}};

	template <class Archive>
	void serialize(Archive& ar) {
		ar.nvp("pointer", pointer, ar.notnull);
		ar.nvp("values", values, ar.notnullrange, ar.notempty, ar.minsize(1), ar.maxsize(2));
		ar.nvp("optional", optional, ar.indirect(ar.notempty), ar.indirect_maxsize(1));
		ar.nvp("nested", nested, ar.ranged(ar.maxsize(2)));
	}
};

// Validators are given after the value: ar(value, validators...). Saving checks them, so the data for load checks is
// saved without validators

TEST_CASE_TEMPLATE("validators", Ar, VIDE_ALL_ARCHIVES) {
	SUBCASE("notnull") {
		CHECK(accepts<Ar>(std::make_unique<int>(42), vide::notnull));
		CHECK(rejects<Ar>(std::unique_ptr<int>{nullptr}, vide::notnull));
	}

	SUBCASE("notnull on a range checks the elements") {
		CHECK(accepts<Ar>(std::vector<std::shared_ptr<int>>{std::make_shared<int>(42)}, vide::notnull));
		CHECK(rejects<Ar>(std::vector<std::shared_ptr<int>>{nullptr}, vide::notnull));
		CHECK(accepts<Ar>(std::vector<int>{1, 2, 3}, vide::notnull));
		CHECK(rejects<Ar>(std::vector<int>{1, 0, 3}, vide::notnull));
	}

	SUBCASE("notnullrange") {
		CHECK(accepts<Ar>(std::vector<std::shared_ptr<int>>{std::make_shared<int>(42)}, vide::notnullrange));
		CHECK(rejects<Ar>(std::vector<std::shared_ptr<int>>{nullptr}, vide::notnullrange));
		CHECK(accepts<Ar>(std::vector<int>{1, 2, 3}, vide::notnullrange));
		CHECK(rejects<Ar>(std::vector<int>{1, 0, 3}, vide::notnullrange));
	}

	SUBCASE("notempty") {
		CHECK(accepts<Ar>(std::vector<int>{0}, vide::notempty));
		CHECK(rejects<Ar>(std::vector<int>{}, vide::notempty));
	}

	SUBCASE("maxsize") {
		CHECK(loads<Ar>(std::vector<int>{}, vide::maxsize(2)));
		CHECK(loads<Ar>(std::vector<int>{0}, vide::maxsize(2)));
		CHECK(loads<Ar>(std::vector<int>{0, 1}, vide::maxsize(2)));
		CHECK_FALSE(loads<Ar>(std::vector<int>{0, 1, 2}, vide::maxsize(2)));
	}

	SUBCASE("minsize") {
		CHECK_FALSE(loads<Ar>(std::vector<int>{}, vide::minsize(2)));
		CHECK_FALSE(loads<Ar>(std::vector<int>{0}, vide::minsize(2)));
		CHECK(loads<Ar>(std::vector<int>{0, 1}, vide::minsize(2)));
		CHECK(loads<Ar>(std::vector<int>{0, 1, 2}, vide::minsize(2)));
	}

	SUBCASE("validators combine in any order") {
		CHECK(accepts<Ar>(41, vide::notnull, is_odd{}));
		CHECK(accepts<Ar>(41, is_odd{}, vide::notnull));
		CHECK(rejects<Ar>(42, vide::notnull, is_odd{}));
		CHECK(rejects<Ar>(42, is_odd{}, vide::notnull));
	}

	SUBCASE("nvp() takes validators") {
		CHECK(accepts_nvp<Ar>(std::make_unique<int>(42), vide::notnull));
		CHECK(rejects_nvp<Ar>(std::unique_ptr<int>{nullptr}, vide::notnull));
	}

	SUBCASE("indirect") {
		CHECK(accepts<Ar>(std::make_unique<int>(41), vide::indirect(vide::notnull), vide::indirect(is_odd{})));
		CHECK(accepts<Ar>(std::make_unique<int>(41), vide::indirect(is_odd{}), vide::indirect(vide::notnull)));
		CHECK(rejects<Ar>(std::make_unique<int>(42), vide::indirect(vide::notnull), vide::indirect(is_odd{})));
		CHECK(rejects<Ar>(std::make_unique<int>(42), vide::indirect(is_odd{}), vide::indirect(vide::notnull)));
		CHECK(accepts<Ar>(std::optional<int>(42), vide::indirect(vide::notnull)));
		CHECK(accepts<Ar>(std::optional<std::vector<int>>{std::in_place}, vide::indirect(vide::maxsize(2))));
		CHECK(accepts<Ar>(std::optional<std::vector<int>>{std::in_place, std::initializer_list<int>{0}}, vide::indirect(vide::maxsize(2))));
		CHECK(accepts<Ar>(std::optional<std::vector<int>>{std::in_place, std::initializer_list<int>{0, 1}}, vide::indirect(vide::maxsize(2))));
		CHECK(rejects<Ar>(std::optional<std::vector<int>>{std::in_place, std::initializer_list<int>{0, 1, 2}}, vide::indirect(vide::maxsize(2))));
	}

	SUBCASE("ranged") {
		CHECK(accepts<Ar>(std::vector<std::vector<int>>{}, vide::ranged(vide::maxsize(2))));
		CHECK(accepts<Ar>(std::vector<std::vector<int>>{std::vector<int>{0}}, vide::ranged(vide::maxsize(2))));
		CHECK(accepts<Ar>(std::vector<std::vector<int>>{std::vector<int>{0}, std::vector<int>{1}, std::vector<int>{2}}, vide::ranged(vide::maxsize(2))));
		CHECK(accepts<Ar>(std::vector<std::vector<int>>{std::vector<int>{0, 1}, std::vector<int>{0, 1}}, vide::ranged(vide::maxsize(2))));
		CHECK(accepts<Ar>(std::vector<std::vector<int>>{std::vector<int>{1, 1}, std::vector<int>{1, 1}}, vide::ranged(vide::notnull)));
		CHECK(rejects<Ar>(std::vector<std::vector<int>>{std::vector<int>{0, 1, 2}}, vide::ranged(vide::maxsize(2))));
		CHECK(rejects<Ar>(std::vector<std::vector<int>>{std::vector<int>{1, 0}}, vide::ranged(vide::notnull)));
		CHECK(rejects<Ar>(std::vector<std::vector<int>>{std::vector<int>{1, 1}, std::vector<int>{0, 1}}, vide::ranged(vide::notnull)));
	}

	SUBCASE("archive dependent names") {
		CHECK(accepts<Ar>(DependentNameValidated{}));
		DependentNameValidated invalid;
		invalid.nested.push_back({1, 2, 3});
		CHECK_FALSE(saves<Ar>(invalid));
	}

	SUBCASE("archives opting out of enforce_validation ignore validators") {
		using Unenforced = proxied<Ar, NonValidatingProxy>;
		CHECK(accepts<Unenforced>(std::make_unique<int>(42), vide::notnull));
		CHECK(accepts<Unenforced>(std::unique_ptr<int>{nullptr}, vide::notnull));
		CHECK(accepts<Unenforced>(std::vector<int>{0}, vide::notempty));
		CHECK(accepts<Unenforced>(std::vector<int>{}, vide::notempty));
		CHECK(accepts<Unenforced>(41, vide::notnull, is_odd{}));
		CHECK(accepts<Unenforced>(41, is_odd{}, vide::notnull));
		CHECK(accepts<Unenforced>(42, vide::notnull, is_odd{}));
		CHECK(accepts<Unenforced>(42, is_odd{}, vide::notnull));
	}

	SUBCASE("verify() throws on failure") {
		std::ostringstream os;
		{
			typename Ar::output ar(os);
			ar(0); // A JSON archive asserts (IsObject()) when closed empty
			CHECK_THROWS_AS(ar.verify(false, "Reason"), vide::Exception);
		}
		std::istringstream is(os.str());
		{
			typename Ar::input ar(is);
			CHECK_THROWS_AS(ar.verify(false, "Reason"), vide::Exception);
		}
	}
}
