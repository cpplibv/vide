//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


TEST_SUITE_BEGIN("validation");


template <typename Ar>
struct NonValidatingProxy : vide::ProxyArchive<NonValidatingProxy<Ar>, Ar> {
	Ar ar;
	static constexpr bool enforce_validation = false;

	template <typename Stream>
	explicit NonValidatingProxy(Stream& stream) : vide::ProxyArchive<NonValidatingProxy<Ar>, Ar>(ar), ar(stream) {}
};

struct is_odd {
	template <typename T>
	inline void operator()(const T& var) const {
		if (var % 2 == 0)
			throw vide::Exception("Non-empty validation failed during serialization: object is empty.");
	}
};

template<typename IArchive, typename OArchive>
struct Tester {
	template <typename T>
	void test_out(const T& varOutput, const auto&... validators) {
		std::ostringstream os; {
			OArchive oar(os);
			oar(varOutput, validators...);
		}
	}

	template <typename T>
	const auto test_in_(const T& varOutput, const auto&... validators) {
		std::ostringstream os; {
			OArchive oar(os);
			oar(varOutput); // No validation on output so we can write out the invalid, so we can test the input validation
		}

		T varInput{};
		std::istringstream is(os.str()); {
			IArchive iar(is);
			iar(varInput, validators...);
		}

		return varInput;
	}
};


template <typename IArchive, typename OArchive>
void test_validation() {
	Tester<IArchive, OArchive> active{};
	// Not null
	CHECK_NOTHROW(active.test_out(std::make_unique<int>(42), vide::notnull));
	CHECK_NOTHROW(active.test_in_(std::make_unique<int>(42), vide::notnull));
	CHECK_THROWS_AS(active.test_out(std::unique_ptr<int>{nullptr}, vide::notnull), vide::Exception);
	CHECK_THROWS_AS(active.test_in_(std::unique_ptr<int>{nullptr}, vide::notnull), vide::Exception);

	// Not null - Range
	CHECK_NOTHROW(active.test_out(std::vector<std::shared_ptr<int>>{std::make_shared<int>(42)}, vide::notnull));
	CHECK_NOTHROW(active.test_in_(std::vector<std::shared_ptr<int>>{std::make_shared<int>(42)}, vide::notnull));
	CHECK_THROWS_AS(active.test_out(std::vector<std::shared_ptr<int>>{nullptr}, vide::notnull), vide::Exception);
	CHECK_THROWS_AS(active.test_in_(std::vector<std::shared_ptr<int>>{nullptr}, vide::notnull), vide::Exception);
	CHECK_NOTHROW(active.test_out(std::vector<int>{1, 2, 3}, vide::notnull));
	CHECK_NOTHROW(active.test_in_(std::vector<int>{1, 2, 3}, vide::notnull));
	CHECK_THROWS_AS(active.test_out(std::vector<int>{1, 0, 3}, vide::notnull), vide::Exception);
	CHECK_THROWS_AS(active.test_in_(std::vector<int>{1, 0, 3}, vide::notnull), vide::Exception);

	// Not null range
	CHECK_NOTHROW(active.test_out(std::vector<std::shared_ptr<int>>{std::make_shared<int>(42)}, vide::notnullrange));
	CHECK_NOTHROW(active.test_in_(std::vector<std::shared_ptr<int>>{std::make_shared<int>(42)}, vide::notnullrange));
	CHECK_THROWS_AS(active.test_out(std::vector<std::shared_ptr<int>>{nullptr}, vide::notnullrange), vide::Exception);
	CHECK_THROWS_AS(active.test_in_(std::vector<std::shared_ptr<int>>{nullptr}, vide::notnullrange), vide::Exception);
	CHECK_NOTHROW(active.test_out(std::vector<int>{1, 2, 3}, vide::notnullrange));
	CHECK_NOTHROW(active.test_in_(std::vector<int>{1, 2, 3}, vide::notnullrange));
	CHECK_THROWS_AS(active.test_out(std::vector<int>{1, 0, 3}, vide::notnullrange), vide::Exception);
	CHECK_THROWS_AS(active.test_in_(std::vector<int>{1, 0, 3}, vide::notnullrange), vide::Exception);

	// Not empty
	CHECK_NOTHROW(active.test_out(std::vector<int>{0}, vide::notempty));
	CHECK_NOTHROW((active.test_in_(std::vector<int>{0}, vide::notempty)));
	CHECK_THROWS_AS(active.test_out(std::vector<int>{}, vide::notempty), vide::Exception);
	CHECK_THROWS_AS(active.test_in_(std::vector<int>{}, vide::notempty), vide::Exception);

	// Max size
	CHECK_NOTHROW(active.test_in_(std::vector<int>{}, vide::maxsize(2)));
	CHECK_NOTHROW(active.test_in_(std::vector<int>{0}, vide::maxsize(2)));
	CHECK_NOTHROW(active.test_in_(std::vector<int>{0, 1}, vide::maxsize(2)));
	CHECK_THROWS_AS(active.test_in_(std::vector<int>{0, 1, 2}, vide::maxsize(2)), vide::Exception);

	// Not null + custom is odd
	CHECK_NOTHROW(active.test_out(41, vide::notnull, is_odd{}));
	CHECK_NOTHROW(active.test_in_(41, vide::notnull, is_odd{}));
	CHECK_NOTHROW(active.test_out(41, is_odd{}, vide::notnull));
	CHECK_NOTHROW(active.test_in_(41, is_odd{}, vide::notnull));
	CHECK_THROWS_AS(active.test_out(42, vide::notnull, is_odd{}), vide::Exception);
	CHECK_THROWS_AS(active.test_in_(42, vide::notnull, is_odd{}), vide::Exception);
	CHECK_THROWS_AS(active.test_out(42, is_odd{}, vide::notnull), vide::Exception);
	CHECK_THROWS_AS(active.test_in_(42, is_odd{}, vide::notnull), vide::Exception);

	Tester<NonValidatingProxy<IArchive>, NonValidatingProxy<OArchive>> inactive{};
	// Not enforced - Not null
	CHECK_NOTHROW(inactive.test_out(std::make_unique<int>(42), vide::notnull));
	CHECK_NOTHROW(inactive.test_in_(std::make_unique<int>(42), vide::notnull));
	CHECK_NOTHROW(inactive.test_out(std::unique_ptr<int>{nullptr}, vide::notnull));
	CHECK_NOTHROW(inactive.test_in_(std::unique_ptr<int>{nullptr}, vide::notnull));

	// Not enforced - Not empty
	CHECK_NOTHROW(inactive.test_out(std::vector<int>{0}, vide::notempty));
	CHECK_NOTHROW((inactive.test_in_(std::vector<int>{0}, vide::notempty)));
	CHECK_NOTHROW(inactive.test_out(std::vector<int>{}, vide::notempty));
	CHECK_NOTHROW(inactive.test_in_(std::vector<int>{}, vide::notempty));

	// Not enforced - Not null + custom is odd
	CHECK_NOTHROW(inactive.test_out(41, vide::notnull, is_odd{}));
	CHECK_NOTHROW(inactive.test_in_(41, vide::notnull, is_odd{}));
	CHECK_NOTHROW(inactive.test_out(41, is_odd{}, vide::notnull));
	CHECK_NOTHROW(inactive.test_in_(41, is_odd{}, vide::notnull));
	CHECK_NOTHROW(inactive.test_out(42, vide::notnull, is_odd{}));
	CHECK_NOTHROW(inactive.test_in_(42, vide::notnull, is_odd{}));
	CHECK_NOTHROW(inactive.test_out(42, is_odd{}, vide::notnull));
	CHECK_NOTHROW(inactive.test_in_(42, is_odd{}, vide::notnull));

	std::ostringstream os; {
		OArchive oar(os);
		oar(0); // To suppress: rapidjson internal assertion failure: IsObject()
		CHECK_THROWS_AS(oar.verify(false, "Reason"), vide::Exception);
	}
	std::istringstream is(os.str()); {
		IArchive iar(is);
		CHECK_THROWS_AS(iar.verify(false, "Reason"), vide::Exception);
	}
}

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("validation", test_validation)

TEST_SUITE_END();
