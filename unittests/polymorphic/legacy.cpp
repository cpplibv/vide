//

#include <vide/types/memory.hpp>
#include <vide/types/polymorphic.hpp>
#include <vide/types/std_string.hpp>
#include <vide/types/std_vector.hpp>

#include <cmath>
#include <future>
#include <memory>
#include <ostream>
#include <random>
#include <string>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "common/serialization_calls.hpp"


namespace legacy { // ------------------------------------------------------------------------------

struct PolyBaseA {
	virtual void foo() = 0;

	virtual ~PolyBaseA() {
	}
};

struct PolyBaseAA : PolyBaseA {
	long w;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(w);
	}

	PolyBaseAA() = default;

	PolyBaseAA(long ww) : w(ww) {
	}

	virtual ~PolyBaseAA() = default;

	void foo() override {
	}

	bool operator==(PolyBaseAA const& other) const {
		return w == other.w;
	}
};
} // namespace legacy ------------------------------------------------------------------------------
VIDE_REGISTER_POLYMORPHIC_RELATION(legacy::PolyBaseA, legacy::PolyBaseAA)
namespace legacy { // ------------------------------------------------------------------------------

struct PolyBaseB : virtual PolyBaseAA {
	int x;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(vide::virtual_base_class<PolyBaseAA>(this));
		ar(x);
	}

	PolyBaseB() = default;

	PolyBaseB(int xx, long ww) : PolyBaseAA(ww), x(xx) {
	}

	virtual ~PolyBaseB() = default;

	bool operator==(PolyBaseB const& other) const {
		return PolyBaseAA::operator==(other) &&
				x == other.x;
	}
};

struct PolyBaseC : virtual PolyBaseAA {
	double y;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(vide::virtual_base_class<PolyBaseAA>(this));
		ar(y);
	}

	PolyBaseC() = default;

	PolyBaseC(double yy, long ww) : PolyBaseAA(ww), y(yy) {
	}

	virtual ~PolyBaseC() = default;

	bool operator==(PolyBaseC const& other) const {
		return PolyBaseAA::operator==(other) &&
				std::abs(y - other.y) < 1e-5;
	}
};

struct PolyDerivedD : PolyBaseB, PolyBaseC {
	std::string z;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(vide::base_class<PolyBaseB>(this));
		ar(vide::base_class<PolyBaseC>(this));
		ar(z);
	}

	PolyDerivedD() = default;

	PolyDerivedD(std::string const& zz, double yy, int xx, long ww) :
		PolyBaseAA(ww), PolyBaseB(xx, ww), PolyBaseC(yy, ww), z(zz) {
	}

	virtual ~PolyDerivedD() = default;

	bool operator==(PolyDerivedD const& other) const {
		return PolyBaseB::operator==(other) &&
				PolyBaseC::operator==(other) &&
				z == other.z;
	}
};
} // namespace legacy ------------------------------------------------------------------------------
VIDE_REGISTER_TYPE(legacy::PolyDerivedD)
namespace legacy { // ------------------------------------------------------------------------------

struct PolyBase {
	int x;
	float y;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(x)(y);
	}

	PolyBase() = default;

	PolyBase(int xx, float yy) : x(xx), y(yy) {
	}

	virtual ~PolyBase() = default;

	virtual void foo() = 0;

	bool operator==(PolyBase const& other) const {
		return x == other.x && std::abs(y - other.y) < 1e-5f;
	}
};

struct PolyDerived : PolyBase {
	bool a;
	double b;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(vide::base_class<PolyBase>(this));
		ar(a)(b);
	}

	PolyDerived() = default;

	PolyDerived(int xx, float yy, bool aa, double bb) :
		PolyBase(xx, yy), a(aa), b(bb) {
	}

	virtual ~PolyDerived() = default;

	bool operator==(PolyDerived const& other) const {
		return PolyBase::operator==(other) && a == other.a && std::abs(b - other.b) < 1e-5;
	}

	void foo() override {
	}
};
} // namespace legacy ------------------------------------------------------------------------------
VIDE_REGISTER_TYPE(legacy::PolyDerived)
namespace legacy { // ------------------------------------------------------------------------------

struct PolyLA : std::enable_shared_from_this<PolyLA> {
	PolyLA() = default;

	virtual ~PolyLA() = default;

	virtual void foo() = 0;
};

struct PolyDerivedLA : public PolyLA {
	int x;
	std::vector<std::shared_ptr<PolyLA>> vec;

	template <class Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(x);
		ar(vec);
	}

	PolyDerivedLA() = default;

	PolyDerivedLA(int xx) : x(xx) {
	}

	virtual ~PolyDerivedLA() = default;

	void foo() override {
	}

	bool operator==(PolyDerivedLA const& other) const {
		if (x != other.x)
			return false;
		if (vec.size() != other.vec.size())
			return false;
		for (size_t i = 0; i < vec.size(); ++i)
			if (!(*std::dynamic_pointer_cast<PolyDerivedLA>(vec[i]) == *std::dynamic_pointer_cast<PolyDerivedLA>(other.vec[i])))
				return false;

		return true;
	}
};
} // namespace legacy ------------------------------------------------------------------------------
VIDE_REGISTER_TYPE(legacy::PolyDerivedLA)
VIDE_REGISTER_POLYMORPHIC_RELATION(legacy::PolyLA, legacy::PolyDerivedLA)
namespace legacy { // ------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, PolyDerivedLA const& s) {
	os << "[x: " << s.x << "] ";
	for (auto const& v : s.vec)
		os << " child: " << (*std::dynamic_pointer_cast<PolyDerivedLA>(v));
	return os;
}

std::ostream& operator<<(std::ostream& os, PolyDerived const& s) {
	os << "[x: " << s.x << " y: " << s.y << " a: " << s.a << " b: " << s.b << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, PolyDerivedD const& s) {
	os << "[w: " << s.w << " x: " << s.x << " y: " << s.y << " z: " << s.z << "]";
	return os;
}

/// Pointees reached through base class pointers, shared, weak and unique, a diamond with a virtual base and a
/// pointee holding pointers of its own
/// The pointee as T; fails the test case unless it is one
template <typename T>
const T& as(const auto& pointer) {
	const auto* derived = dynamic_cast<const T*>(pointer.get());
	REQUIRE(derived);
	return *derived;
}

template <typename Ar>
void check_legacy_roundtrip(std::mt19937& gen) {
	const auto random_derived = [&] {
		const int x = random_value<int>(gen);
		const float y = random_value<float>(gen);
		const bool a = random_value<bool>(gen);
		return PolyDerived{x, y, a, random_value<double>(gen)};
	};
	const auto random_diamond = [&] {
		std::string z = random_value<std::string>(gen);
		const double y = random_value<double>(gen);
		const int x = random_value<int>(gen);
		return PolyDerivedD{z, y, x, random_value<long>(gen)};
	};
	const auto random_holder = [&] { // Holds one child
		auto holder = std::make_shared<PolyDerivedLA>(random_value<int>(gen));
		holder->vec.push_back(std::make_shared<PolyDerivedLA>(random_value<int>(gen)));
		return holder;
	};

	const std::shared_ptr<PolyBase> shared = std::make_shared<PolyDerived>(random_derived());
	const std::shared_ptr<const PolyBase> shared_const = std::make_shared<const PolyDerived>(random_derived());
	const std::weak_ptr<PolyBase> weak = shared;
	const std::weak_ptr<const PolyBase> weak_const = shared_const;
	const std::unique_ptr<PolyBase> unique = std::make_unique<PolyDerived>(random_derived());
	const std::unique_ptr<const PolyBase> unique_const = std::make_unique<const PolyDerived>(random_derived());
	const std::shared_ptr<PolyLA> holder = random_holder();
	const std::shared_ptr<const PolyLA> holder_const = random_holder();
	const std::shared_ptr<PolyBaseA> diamond = std::make_shared<PolyDerivedD>(random_diamond());
	const std::weak_ptr<PolyBaseA> weak_diamond = diamond;
	const std::unique_ptr<PolyBaseA> unique_diamond = std::make_unique<PolyDerivedD>(random_diamond());

	const CallLog calls;
	const auto [loaded_shared, loaded_shared_const, loaded_weak, loaded_weak_const, loaded_unique, loaded_unique_const,
			loaded_holder, loaded_holder_const, loaded_diamond, loaded_weak_diamond, loaded_unique_diamond] =
			roundtrip<Ar>(shared, shared_const, weak, weak_const, unique, unique_const, holder, holder_const, diamond,
					weak_diamond, unique_diamond);

	CHECK_EQ(as<PolyDerived>(loaded_shared), as<PolyDerived>(shared));
	CHECK_EQ(as<PolyDerived>(loaded_shared_const), as<PolyDerived>(shared_const));
	CHECK_EQ(loaded_weak.lock(), loaded_shared);
	CHECK_EQ(loaded_weak_const.lock(), loaded_shared_const);
	CHECK_EQ(as<PolyDerived>(loaded_unique), as<PolyDerived>(unique));
	CHECK_EQ(as<PolyDerived>(loaded_unique_const), as<PolyDerived>(unique_const));

	CHECK_EQ(as<PolyDerivedLA>(loaded_holder), as<PolyDerivedLA>(holder));
	CHECK_EQ(as<PolyDerivedLA>(loaded_holder_const), as<PolyDerivedLA>(holder_const));
	CHECK_EQ(loaded_holder->shared_from_this(), loaded_holder);

	CHECK_EQ(as<PolyDerivedD>(loaded_diamond), as<PolyDerivedD>(diamond));
	CHECK_EQ(loaded_weak_diamond.lock(), loaded_diamond);
	CHECK_EQ(as<PolyDerivedD>(loaded_unique_diamond), as<PolyDerivedD>(unique_diamond));

	// Weak pointers reach no new object; a virtual base runs once per object, not once per path to it
	calls.check<PolyDerived>(4);
	calls.check<PolyBase>(4);
	calls.check<PolyDerivedLA>(4);
	calls.check<PolyDerivedD>(2);
	calls.check<PolyBaseB>(2);
	calls.check<PolyBaseC>(2);
	calls.check<PolyBaseAA>(2);
}

} // namespace legacy ------------------------------------------------------------------------------

TEST_CASE_TEMPLATE("polymorphic legacy", Ar, VIDE_ALL_ARCHIVES) {
	SUBCASE("pointer kinds and hierarchies") {
		auto gen = test_rng();
		legacy::check_legacy_roundtrip<Ar>(gen);
	}

	SUBCASE("concurrent archives") { // The polymorphic registries are shared between threads
		constexpr unsigned thread_count = 32;
		std::vector<std::future<void>> pool;
		for (unsigned i = 0; i < thread_count; ++i)
			pool.push_back(std::async(std::launch::async, [seed = test_rng()() + i] {
				std::mt19937 gen{seed};
				legacy::check_legacy_roundtrip<Ar>(gen);
			}));
		for (auto& future : pool)
			CHECK_NOTHROW(future.get());
	}
}
