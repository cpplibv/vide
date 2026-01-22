//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <source_location>

#include "common.hpp"

#if VIDE_THREAD_SAFE
#include <future>
#endif

#include <iostream>

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
		ar(w);
	}

	PolyBaseAA() = default;

	PolyBaseAA(long ww) : w(ww) {
	}

	virtual ~PolyBaseAA() = default;

	void foo() {
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
		ar(x)(y);
	}

	PolyBase() = default;

	PolyBase(int xx, float yy) : x(xx), y(yy) {
	}

	virtual ~PolyBase() = default;

	virtual void foo() = 0;

	bool operator==(PolyBase const& other) const {
		return x == other.x && std::abs(y - other.y) < 1e-5;
	}
};

struct PolyDerived : PolyBase {
	bool a;
	double b;

	template <class Archive>
	void serialize(Archive& ar) {
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

	void foo() {
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
		ar(x);
		ar(vec);
	}

	PolyDerivedLA() = default;

	PolyDerivedLA(int xx) : x(xx) {
	}

	virtual ~PolyDerivedLA() = default;

	void foo() {
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

template <class IArchive, class OArchive>
void test_polymorphic_legacy() {
	std::mt19937 rng(0x5EED);

	auto rngB = [&]() { return random_value<int>(rng) % 2 == 0; };
	auto rngI = [&]() { return random_value<int>(rng); };
	auto rngL = [&]() { return random_value<long>(rng); };
	auto rngF = [&]() { return random_value<float>(rng); };
	auto rngD = [&]() { return random_value<double>(rng); };

	for (int ii = 0; ii < 100; ++ii) {
		std::shared_ptr<PolyBase> o_shared = std::make_shared<PolyDerived>(rngI(), rngF(), rngB(), rngD());
		std::shared_ptr<const PolyBase> o_sharedC = std::make_shared<const PolyDerived>(rngI(), rngF(), rngB(), rngD());

		std::weak_ptr<PolyBase> o_weak = o_shared;
		std::weak_ptr<const PolyBase> o_weakC = o_sharedC;

		std::unique_ptr<PolyBase> o_unique(new PolyDerived(rngI(), rngF(), rngB(), rngD()));
		std::unique_ptr<const PolyBase> o_uniqueC(new PolyDerived(rngI(), rngF(), rngB(), rngD()));

		std::shared_ptr<PolyBaseA> o_sharedA = std::make_shared<PolyDerivedD>(random_basic_string<char>(rng),
				rngD(), rngI(), rngL());

		std::weak_ptr<PolyBaseA> o_weakA = o_sharedA;
		std::unique_ptr<PolyBaseA> o_uniqueA(new PolyDerivedD(random_basic_string<char>(rng),
				rngD(), rngI(), rngL()));

		auto pda = std::make_shared<PolyDerivedLA>(rngI());
		pda->vec.emplace_back(std::make_shared<PolyDerivedLA>(rngI()));
		std::shared_ptr<PolyLA> o_sharedLA = pda;

		auto pdaC = std::make_shared<const PolyDerivedLA>(rngI());
		pda->vec.emplace_back(std::make_shared<PolyDerivedLA>(rngI()));
		std::shared_ptr<const PolyLA> o_sharedLAC = pdaC;

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_shared);
			oar(o_sharedC);
			oar(o_weak);
			oar(o_weakC);
			oar(o_unique);
			oar(o_uniqueC);

			oar(o_sharedLA);
			oar(o_sharedLAC);

			oar(o_sharedA);
			oar(o_weakA);
			oar(o_uniqueA);
		}

		// std::cout << std::endl;
		// std::cout << "========" << std::endl;
		// // std::cout << os.str() << std::endl;
		// std::cout << hex_dump_with_ascii(os.str()) << std::endl;

		decltype(o_shared) i_shared;
		decltype(o_sharedC) i_sharedC;

		decltype(o_weak) i_weak;
		decltype(o_weakC) i_weakC;

		decltype(o_unique) i_unique;
		decltype(o_uniqueC) i_uniqueC;

		decltype(o_sharedLA) i_sharedLA;
		decltype(o_sharedLAC) i_sharedLAC;

		decltype(o_sharedA) i_sharedA;
		decltype(o_weakA) i_weakA;
		decltype(o_uniqueA) i_uniqueA;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_shared);
			iar(i_sharedC);
			iar(i_weak);
			iar(i_weakC);
			iar(i_unique);
			iar(i_uniqueC);

			iar(i_sharedLA);
			iar(i_sharedLAC);

			iar(i_sharedA);
			iar(i_weakA);
			iar(i_uniqueA);
		}

		auto i_locked = i_weak.lock();
		auto o_locked = o_weak.lock();

		auto i_lockedC = i_weakC.lock();
		auto o_lockedC = o_weakC.lock();

		auto i_sharedLA2 = i_sharedLA->shared_from_this();
		auto i_sharedLA2C = i_sharedLAC->shared_from_this();

		auto i_lockedA = i_weakA.lock();
		auto o_lockedA = o_weakA.lock();

		CHECK_EQ(i_shared.get(), i_locked.get());
		CHECK_EQ(*dynamic_cast<PolyDerived*>(i_shared.get()), *dynamic_cast<PolyDerived*>(o_shared.get()));
		CHECK_EQ(*dynamic_cast<PolyDerived*>(i_shared.get()), *dynamic_cast<PolyDerived*>(i_locked.get()));
		CHECK_EQ(*dynamic_cast<PolyDerived*>(i_locked.get()), *dynamic_cast<PolyDerived*>(o_locked.get()));
		CHECK_EQ(*dynamic_cast<PolyDerived*>(i_unique.get()), *dynamic_cast<PolyDerived*>(o_unique.get()));

		CHECK_EQ(i_sharedC.get(), i_lockedC.get());
		CHECK_EQ(*dynamic_cast<const PolyDerived*>(i_sharedC.get()), *dynamic_cast<const PolyDerived*>(o_sharedC.get()));
		CHECK_EQ(*dynamic_cast<const PolyDerived*>(i_sharedC.get()), *dynamic_cast<const PolyDerived*>(i_lockedC.get()));
		CHECK_EQ(*dynamic_cast<const PolyDerived*>(i_lockedC.get()), *dynamic_cast<const PolyDerived*>(o_lockedC.get()));
		CHECK_EQ(*dynamic_cast<const PolyDerived*>(i_uniqueC.get()), *dynamic_cast<const PolyDerived*>(o_uniqueC.get()));

		CHECK_EQ(*dynamic_cast<PolyDerivedLA*>(i_sharedLA.get()), *dynamic_cast<PolyDerivedLA*>(o_sharedLA.get()));
		CHECK_EQ(*dynamic_cast<PolyDerivedLA*>(i_sharedLA2.get()), *dynamic_cast<PolyDerivedLA*>(o_sharedLA.get()));

		CHECK_EQ(*dynamic_cast<const PolyDerivedLA*>(i_sharedLAC.get()), *dynamic_cast<const PolyDerivedLA*>(o_sharedLAC.get()));
		CHECK_EQ(*dynamic_cast<const PolyDerivedLA*>(i_sharedLA2C.get()), *dynamic_cast<const PolyDerivedLA*>(o_sharedLAC.get()));

		CHECK_EQ(i_sharedA.get(), i_lockedA.get());
		CHECK_EQ(*dynamic_cast<PolyDerivedD*>(i_sharedA.get()), *dynamic_cast<PolyDerivedD*>(o_sharedA.get()));
		CHECK_EQ(*dynamic_cast<PolyDerivedD*>(i_sharedA.get()), *dynamic_cast<PolyDerivedD*>(i_lockedA.get()));
		CHECK_EQ(*dynamic_cast<PolyDerivedD*>(i_lockedA.get()), *dynamic_cast<PolyDerivedD*>(o_lockedA.get()));
		CHECK_EQ(*dynamic_cast<PolyDerivedD*>(i_uniqueA.get()), *dynamic_cast<PolyDerivedD*>(o_uniqueA.get()));
	}
}

#if VIDE_THREAD_SAFE
template <class IArchive, class OArchive>
inline
void test_polymorphic_threading() {
	std::vector<std::future<bool>> pool;
	for (size_t i = 0; i < 100; ++i)
		pool.emplace_back(std::async(std::launch::async,
				[]() {
					test_polymorphic<IArchive, OArchive>();
					return true;
				}));

	for (auto& future : pool)
		future.wait();

	for (auto& future : pool)
		CHECK_UNARY(future.get());
}
#endif // VIDE_THREAD_SAFE

} // namespace legacy ------------------------------------------------------------------------------

TEST_SUITE_BEGIN("polymorphic");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic_legacy", legacy::test_polymorphic_legacy)

#if VIDE_THREAD_SAFE
CREATE_TEST_CASES_FOR_ALL_ARCHIVE ("polymorphic_threading", legacy::test_polymorphic_threading)
#endif // VIDE_THREAD_SAFE

TEST_SUITE_END();

