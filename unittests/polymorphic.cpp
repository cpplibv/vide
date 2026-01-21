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


// friend bool operator==(const RootSimpleB& lhs, const RootSimpleB& rhs) {
// 	return lhs.dataRootSimpleB == rhs.dataRootSimpleB;
// }

/// Starts with a random value, each member increments it by 1, each base receives a value incremented by 1000000
struct IV {
	int value = 0;

	explicit IV(int value) :
		value(value) {
	}

	IV(IV& other) :
		value(other.value + 1000000) {
		++other.value; // Abuse "copy constructor" which is only called when passing to base to count as 1 member
	}
	IV(const IV& other) = delete;

	explicit operator int() {
		return value++;
	}
};

struct Initializer {
	std::mt19937& rng;

	explicit Initializer(std::mt19937& rng) : rng(rng) { }

	explicit(false) operator IV() const {
		return IV{std::abs(random_value<int>(rng) % 1000) + 1000000};
	}
};

// -------------------------------------------------------------------------------------------------

struct RootRegularEmptyA {
	mutable int usedSerializeFunction = 0;
	template <typename Archive> void serialize(Archive&) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
	}
	[[nodiscard]] bool operator==(const RootRegularEmptyA& other) const noexcept = default;
};
struct RootRegularEmptyB {
	mutable int usedSerializeFunction = 0;
	template <typename Archive> void serialize(Archive&) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
	}
	[[nodiscard]] bool operator==(const RootRegularEmptyB& other) const noexcept = default;
};
struct RootRegularEmptyC {
	mutable int usedSerializeFunction = 0;
	template <typename Archive> void serialize(Archive&) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
	}
	[[nodiscard]] bool operator==(const RootRegularEmptyC& other) const noexcept = default;
};

// -------------------------------------------------------------------------------------------------

struct RootRegularSimpleA {
	mutable int usedSerializeFunction = 0;
	int dataRootRegularSimpleA = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.nvp("dataRootRegularSimpleA", dataRootRegularSimpleA);
	}
	RootRegularSimpleA() = default;
	explicit RootRegularSimpleA(IV v) : dataRootRegularSimpleA(v) {}
	virtual ~RootRegularSimpleA() = default;
	[[nodiscard]] bool operator==(const RootRegularSimpleA& other) const noexcept = default;
};
struct RootRegularSimpleB {
	mutable int usedSerializeFunction = 0;
	int dataRootRegularSimpleB = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.nvp("dataRootRegularSimpleB", dataRootRegularSimpleB);
	}
	RootRegularSimpleB() = default;
	explicit RootRegularSimpleB(IV v) : dataRootRegularSimpleB(v) {}
	virtual ~RootRegularSimpleB() = default;
	[[nodiscard]] bool operator==(const RootRegularSimpleB& other) const noexcept = default;
};
struct RootRegularSimpleC {
	mutable int usedSerializeFunction = 0;
	int dataRootRegularSimpleC = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.nvp("dataRootRegularSimpleC", dataRootRegularSimpleC);
	}
	RootRegularSimpleC() = default;
	explicit RootRegularSimpleC(IV v) : dataRootRegularSimpleC(v) {}
	virtual ~RootRegularSimpleC() = default;
	[[nodiscard]] bool operator==(const RootRegularSimpleC& other) const noexcept = default;
};

// -------------------------------------------------------------------------------------------------

struct RootEmptyA {
	mutable int usedSerializeFunction = 0;
	template <typename Archive> void serialize(Archive&) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
	}
	virtual ~RootEmptyA() = default;
	[[nodiscard]] bool operator==(const RootEmptyA& other) const noexcept = default;
};
struct RootEmptyB {
	mutable int usedSerializeFunction = 0;
	template <typename Archive> void serialize(Archive&) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
	}
	virtual ~RootEmptyB() = default;
	[[nodiscard]] bool operator==(const RootEmptyB& other) const noexcept = default;
};
struct RootEmptyC {
	mutable int usedSerializeFunction = 0;
	template <typename Archive> void serialize(Archive&) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
	}
	virtual ~RootEmptyC() = default;
	[[nodiscard]] bool operator==(const RootEmptyC& other) const noexcept = default;
};

// -------------------------------------------------------------------------------------------------

struct RootSimpleA {
	mutable int usedSerializeFunction = 0;
	int dataRootSimpleA = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.nvp("dataRootSimpleA", dataRootSimpleA);
	}
	RootSimpleA() = default;
	explicit RootSimpleA(IV v) : dataRootSimpleA(v) {}
	virtual ~RootSimpleA() = default;
	[[nodiscard]] bool operator==(const RootSimpleA& other) const noexcept = default;
};
struct RootSimpleB {
	mutable int usedSerializeFunction = 0;
	int dataRootSimpleB = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.nvp("dataRootSimpleB", dataRootSimpleB);
	}
	RootSimpleB() = default;
	explicit RootSimpleB(IV v) : dataRootSimpleB(v) {}
	virtual ~RootSimpleB() = default;
	[[nodiscard]] bool operator==(const RootSimpleB& other) const noexcept = default;
};
struct RootSimpleC {
	mutable int usedSerializeFunction = 0;
	int dataRootSimpleC = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.nvp("dataRootSimpleC", dataRootSimpleC);
	}
	RootSimpleC() = default;
	explicit RootSimpleC(IV v) : dataRootSimpleC(v) {}
	virtual ~RootSimpleC() = default;
	[[nodiscard]] bool operator==(const RootSimpleC& other) const noexcept = default;
};

// -------------------------------------------------------------------------------------------------

struct LeafFinal_EmptyA final : RootEmptyA {
	mutable int usedSerializeFunction = 0;
	int dataLeafFinal_EmptyA = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<RootEmptyA>(this);
		ar.nvp("dataLeafFinal_EmptyA", dataLeafFinal_EmptyA);
	}
	LeafFinal_EmptyA() = default;
	explicit LeafFinal_EmptyA(IV v) : dataLeafFinal_EmptyA(v) {}
	virtual ~LeafFinal_EmptyA() = default;
	[[nodiscard]] bool operator==(const LeafFinal_EmptyA& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(LeafFinal_EmptyA)

struct LeafFinal_SimpleA final : RootSimpleA {
	mutable int usedSerializeFunction = 0;
	int dataLeafFinal_SimpleA = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<RootSimpleA>(this);
		ar.nvp("dataLeafFinal_SimpleA", dataLeafFinal_SimpleA);
	}
	LeafFinal_SimpleA() = default;
	explicit LeafFinal_SimpleA(IV v) : RootSimpleA(v), dataLeafFinal_SimpleA(v) {}
	virtual ~LeafFinal_SimpleA() = default;
	[[nodiscard]] bool operator==(const LeafFinal_SimpleA& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(LeafFinal_SimpleA)

// -------------------------------------------------------------------------------------------------

struct LeafNonFinal_EmptyA : RootEmptyA {
	mutable int usedSerializeFunction = 0;
	int dataLeafNonFinal_EmptyA = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<RootEmptyA>(this);
		ar.nvp("dataLeafNonFinal_EmptyA", dataLeafNonFinal_EmptyA);
	}
	LeafNonFinal_EmptyA() = default;
	explicit LeafNonFinal_EmptyA(IV v) : dataLeafNonFinal_EmptyA(v) {}
	virtual ~LeafNonFinal_EmptyA() = default;
	[[nodiscard]] bool operator==(const LeafNonFinal_EmptyA& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(LeafNonFinal_EmptyA)

struct LeafNonFinalA_SimpleA : RootSimpleA {
	mutable int usedSerializeFunction = 0;
	int dataLeafNonFinalA_SimpleA = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<RootSimpleA>(this);
		ar.nvp("dataLeafNonFinalA_SimpleA", dataLeafNonFinalA_SimpleA);
	}
	LeafNonFinalA_SimpleA() = default;
	explicit LeafNonFinalA_SimpleA(IV v) : RootSimpleA(v), dataLeafNonFinalA_SimpleA(v) {}
	virtual ~LeafNonFinalA_SimpleA() = default;
	[[nodiscard]] bool operator==(const LeafNonFinalA_SimpleA& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(LeafNonFinalA_SimpleA)

struct LeafNonFinalB_SimpleA : RootSimpleA {
	mutable int usedSerializeFunction = 0;
	int dataLeafNonFinalB_SimpleA = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<RootSimpleA>(this);
		ar.nvp("dataLeafNonFinalB_SimpleA", dataLeafNonFinalB_SimpleA);
	}
	LeafNonFinalB_SimpleA() = default;
	explicit LeafNonFinalB_SimpleA(IV v) : RootSimpleA(v), dataLeafNonFinalB_SimpleA(v) {}
	virtual ~LeafNonFinalB_SimpleA() = default;
	[[nodiscard]] bool operator==(const LeafNonFinalB_SimpleA& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(LeafNonFinalB_SimpleA)

// -------------------------------------------------------------------------------------------------

struct PreamblePadding {
	mutable int usedSerializeFunction = 0;
	int dataPreamblePadding = 42; // This "padding" is not serialized
	[[nodiscard]] bool operator==(const PreamblePadding& other) const noexcept = default;
};

// Tests base pointer != object address
struct BaseAtNonZeroOffset final : PreamblePadding, RootSimpleA {
	mutable int usedSerializeFunction = 0;
	int dataBaseAtNonZeroOffset = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<RootSimpleA>(this);
		ar.nvp("dataBaseAtNonZeroOffset", dataBaseAtNonZeroOffset);
	}
	BaseAtNonZeroOffset() = default;
	explicit BaseAtNonZeroOffset(IV v) : RootSimpleA(v), dataBaseAtNonZeroOffset(v) {}
	virtual ~BaseAtNonZeroOffset() = default;
	[[nodiscard]] bool operator==(const BaseAtNonZeroOffset& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(BaseAtNonZeroOffset)

// -------------------------------------------------------------------------------------------------

struct MultipleNonVirtualBases final : RootSimpleA, RootSimpleB {
	mutable int usedSerializeFunction = 0;
	int dataMultipleNonVirtualBases = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<RootSimpleA>(this);
		ar.template base_class<RootSimpleB>(this);
		ar.nvp("dataMultipleNonVirtualBases", dataMultipleNonVirtualBases);
	}
	MultipleNonVirtualBases() = default;
	explicit MultipleNonVirtualBases(IV v) :
		RootSimpleA(v),
		RootSimpleB(v),
		dataMultipleNonVirtualBases(v) {}
	virtual ~MultipleNonVirtualBases() = default;
	[[nodiscard]] bool operator==(const MultipleNonVirtualBases& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(MultipleNonVirtualBases)

struct MultipleNonVirtualReverseBases final : RootSimpleA, RootSimpleB {
	mutable int usedSerializeFunction = 0;
	int dataMultipleNonVirtualReverseBases = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<RootSimpleB>(this); // < Reversed listing of bases
		ar.template base_class<RootSimpleA>(this);
		ar.nvp("dataMultipleNonVirtualReverseBases", dataMultipleNonVirtualReverseBases);
	}
	MultipleNonVirtualReverseBases() = default;
	explicit MultipleNonVirtualReverseBases(IV v) :
		RootSimpleA(v),
		RootSimpleB(v),
		dataMultipleNonVirtualReverseBases(v) {}
	virtual ~MultipleNonVirtualReverseBases() = default;
	[[nodiscard]] bool operator==(const MultipleNonVirtualReverseBases& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(MultipleNonVirtualReverseBases)

// -------------------------------------------------------------------------------------------------

struct SingleVirtualBaseA_SimpleA : virtual RootSimpleA {
	mutable int usedSerializeFunction = 0;
	int dataSingleVirtualBaseA_SimpleA = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template virtual_base_class<RootSimpleA>(this);
		ar.nvp("dataSingleVirtualBaseA_SimpleA", dataSingleVirtualBaseA_SimpleA);
	}
	SingleVirtualBaseA_SimpleA() = default;
	explicit SingleVirtualBaseA_SimpleA(IV v) :
		RootSimpleA(v),
		dataSingleVirtualBaseA_SimpleA(v) {}
	virtual ~SingleVirtualBaseA_SimpleA() = default;
	[[nodiscard]] bool operator==(const SingleVirtualBaseA_SimpleA& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(SingleVirtualBaseA_SimpleA)

struct SingleVirtualBaseB_SimpleA : virtual RootSimpleA {
	mutable int usedSerializeFunction = 0;
	int dataSingleVirtualBaseB_SimpleA = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template virtual_base_class<RootSimpleA>(this);
		ar.nvp("dataSingleVirtualBaseB_SimpleA", dataSingleVirtualBaseB_SimpleA);
	}
	SingleVirtualBaseB_SimpleA() = default;
	explicit SingleVirtualBaseB_SimpleA(IV v) :
		RootSimpleA(v),
		dataSingleVirtualBaseB_SimpleA(v) {}
	virtual ~SingleVirtualBaseB_SimpleA() = default;
	[[nodiscard]] bool operator==(const SingleVirtualBaseB_SimpleA& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(SingleVirtualBaseB_SimpleA)

// -------------------------------------------------------------------------------------------------

struct MultipleIndependentVirtualBase : virtual RootSimpleA, virtual RootSimpleB, virtual RootSimpleC {
	mutable int usedSerializeFunction = 0;
	int dataMultipleIndependentVirtualBase = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template virtual_base_class<RootSimpleA>(this);
		ar.template virtual_base_class<RootSimpleB>(this);
		ar.template virtual_base_class<RootSimpleC>(this);
		ar.nvp("dataMultipleIndependentVirtualBase", dataMultipleIndependentVirtualBase);
	}
	MultipleIndependentVirtualBase() = default;
	explicit MultipleIndependentVirtualBase(IV v) :
		RootSimpleA(v),
		RootSimpleB(v),
		RootSimpleC(v),
		dataMultipleIndependentVirtualBase(v) {}
	virtual ~MultipleIndependentVirtualBase() = default;
	[[nodiscard]] bool operator==(const MultipleIndependentVirtualBase& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(MultipleIndependentVirtualBase)

// -------------------------------------------------------------------------------------------------

struct VirtualBaseAfterNonVirtualBase final : RootSimpleA, virtual RootSimpleB {
	mutable int usedSerializeFunction = 0;
	int dataVirtualBaseAfterNonVirtualBase = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<RootSimpleA>(this);
		ar.template virtual_base_class<RootSimpleB>(this);
		ar.nvp("dataVirtualBaseAfterNonVirtualBase", dataVirtualBaseAfterNonVirtualBase);
	}
	VirtualBaseAfterNonVirtualBase() = default;
	explicit VirtualBaseAfterNonVirtualBase(IV v) :
		RootSimpleB(v), // virtual should be initialized first
		RootSimpleA(v),
		dataVirtualBaseAfterNonVirtualBase(v) {}
	virtual ~VirtualBaseAfterNonVirtualBase() = default;
	[[nodiscard]] bool operator==(const VirtualBaseAfterNonVirtualBase& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(VirtualBaseAfterNonVirtualBase)

struct NonVirtualBaseAfterVirtualBase final : virtual RootSimpleA, RootSimpleB {
	mutable int usedSerializeFunction = 0;
	int dataNonVirtualBaseAfterVirtualBase = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template virtual_base_class<RootSimpleA>(this);
		ar.template base_class<RootSimpleB>(this);
		ar.nvp("dataNonVirtualBaseAfterVirtualBase", dataNonVirtualBaseAfterVirtualBase);
	}
	NonVirtualBaseAfterVirtualBase() = default;
	explicit NonVirtualBaseAfterVirtualBase(IV v) :
		RootSimpleA(v),
		RootSimpleB(v),
		dataNonVirtualBaseAfterVirtualBase(v) {}
	virtual ~NonVirtualBaseAfterVirtualBase() = default;
	[[nodiscard]] bool operator==(const NonVirtualBaseAfterVirtualBase& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(NonVirtualBaseAfterVirtualBase)

// Macro or X Macro madness
// CREATE_CLAZZ_0(VirtualBaseAfterNonVirtualBase, RootSimpleA, virtual RootSimpleB)
// 	ar.template base_class<RootSimpleA>(this);
// 	ar.template virtual_base_class<RootSimpleB>(this);
// CREATE_CLAZZ_1()
// 		RootSimpleA(v),
// 		RootSimpleB(v),
// CREATE_CLAZZ_2()
// 	[[nodiscard]] bool operator==(const VirtualBaseAfterNonVirtualBase& other) const noexcept = default;
// CREATE_CLAZZ_END()

// -------------------------------------------------------------------------------------------------

struct ClassicVirtualDiamond final : SingleVirtualBaseA_SimpleA, SingleVirtualBaseB_SimpleA {
	mutable int usedSerializeFunction = 0;
	int dataClassicVirtualDiamond = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<SingleVirtualBaseA_SimpleA>(this);
		ar.template base_class<SingleVirtualBaseB_SimpleA>(this);
		ar.nvp("dataClassicVirtualDiamond", dataClassicVirtualDiamond);
	}
	ClassicVirtualDiamond() = default;
	explicit ClassicVirtualDiamond(IV v) :
		RootSimpleA(v),
		SingleVirtualBaseA_SimpleA(v),
		SingleVirtualBaseB_SimpleA(v),
		dataClassicVirtualDiamond(v) {}
	virtual ~ClassicVirtualDiamond() = default;
	[[nodiscard]] bool operator==(const ClassicVirtualDiamond& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(ClassicVirtualDiamond)

struct LeftVirtualVirtualDiamond final : virtual SingleVirtualBaseA_SimpleA, SingleVirtualBaseB_SimpleA {
	mutable int usedSerializeFunction = 0;
	int dataLeftVirtualVirtualDiamond = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template virtual_base_class<SingleVirtualBaseA_SimpleA>(this);
		ar.template base_class<SingleVirtualBaseB_SimpleA>(this);
		ar.nvp("dataLeftVirtualVirtualDiamond", dataLeftVirtualVirtualDiamond);
	}
	LeftVirtualVirtualDiamond() = default;
	explicit LeftVirtualVirtualDiamond(IV v) :
		RootSimpleA(v),
		SingleVirtualBaseA_SimpleA(v),
		SingleVirtualBaseB_SimpleA(v),
		dataLeftVirtualVirtualDiamond(v) {}
	virtual ~LeftVirtualVirtualDiamond() = default;
	[[nodiscard]] bool operator==(const LeftVirtualVirtualDiamond& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(LeftVirtualVirtualDiamond)

struct RightVirtualVirtualDiamond final : SingleVirtualBaseA_SimpleA, virtual SingleVirtualBaseB_SimpleA {
	mutable int usedSerializeFunction = 0;
	int dataRightVirtualVirtualDiamond = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<SingleVirtualBaseA_SimpleA>(this);
		ar.template virtual_base_class<SingleVirtualBaseB_SimpleA>(this);
		ar.nvp("dataRightVirtualVirtualDiamond", dataRightVirtualVirtualDiamond);
	}
	RightVirtualVirtualDiamond() = default;
	explicit RightVirtualVirtualDiamond(IV v) :
		RootSimpleA(v),
		SingleVirtualBaseB_SimpleA(v), // virtual should be initialized first
		SingleVirtualBaseA_SimpleA(v),
		dataRightVirtualVirtualDiamond(v) {}
	virtual ~RightVirtualVirtualDiamond() = default;
	[[nodiscard]] bool operator==(const RightVirtualVirtualDiamond& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(RightVirtualVirtualDiamond)

struct BothVirtualVirtualDiamond final : virtual SingleVirtualBaseA_SimpleA, virtual SingleVirtualBaseB_SimpleA {
	mutable int usedSerializeFunction = 0;
	int dataBothVirtualVirtualDiamond = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template virtual_base_class<SingleVirtualBaseA_SimpleA>(this);
		ar.template virtual_base_class<SingleVirtualBaseB_SimpleA>(this);
		ar.nvp("dataBothVirtualVirtualDiamond", dataBothVirtualVirtualDiamond);
	}
	BothVirtualVirtualDiamond() = default;
	explicit BothVirtualVirtualDiamond(IV v) :
		RootSimpleA(v),
		SingleVirtualBaseA_SimpleA(v),
		SingleVirtualBaseB_SimpleA(v),
		dataBothVirtualVirtualDiamond(v) {}
	virtual ~BothVirtualVirtualDiamond() = default;
	[[nodiscard]] bool operator==(const BothVirtualVirtualDiamond& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(BothVirtualVirtualDiamond)

// -------------------------------------------------------------------------------------------------

struct MixedDepth : LeafNonFinalA_SimpleA, RootSimpleB {
	mutable int usedSerializeFunction = 0;
	int dataMixedDepth = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<LeafNonFinalA_SimpleA>(this);
		ar.template base_class<RootSimpleB>(this);
		ar.nvp("dataMixedDepth", dataMixedDepth);
	}
	MixedDepth() = default;
	explicit MixedDepth(IV v) :
		LeafNonFinalA_SimpleA(v),
		RootSimpleB(v),
		dataMixedDepth(v) {}
	virtual ~MixedDepth() = default;
	[[nodiscard]] bool operator==(const MixedDepth& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(MixedDepth)

// -------------------------------------------------------------------------------------------------

struct DuplicateNonVirtualBase final : LeafNonFinalA_SimpleA, LeafNonFinalB_SimpleA {
	mutable int usedSerializeFunction = 0;
	int dataDuplicateNonVirtualBase = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<LeafNonFinalA_SimpleA>(this);
		ar.template base_class<LeafNonFinalB_SimpleA>(this);
		ar.nvp("dataDuplicateNonVirtualBase", dataDuplicateNonVirtualBase);
	}
	DuplicateNonVirtualBase() = default;
	explicit DuplicateNonVirtualBase(IV v) :
		LeafNonFinalA_SimpleA(v),
		LeafNonFinalB_SimpleA(v),
		dataDuplicateNonVirtualBase(v) {}
	virtual ~DuplicateNonVirtualBase() = default;
	[[nodiscard]] bool operator==(const DuplicateNonVirtualBase& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(DuplicateNonVirtualBase)

// -------------------------------------------------------------------------------------------------

struct DeepChain1 {
	mutable int usedSerializeFunction = 0;
	int dataDeepChain1 = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.nvp("dataDeepChain1", dataDeepChain1);
	}
	DeepChain1() = default;
	explicit DeepChain1(IV v) :
		dataDeepChain1(v) {}
	virtual ~DeepChain1() = default;
	[[nodiscard]] bool operator==(const DeepChain1& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(DeepChain1)

struct DeepChain2 : DeepChain1 {
	mutable int usedSerializeFunction = 0;
	int dataDeepChain2 = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<DeepChain1>(this);
		ar.nvp("dataDeepChain2", dataDeepChain2);
	}
	DeepChain2() = default;
	explicit DeepChain2(IV v) :
		DeepChain1(v),
		dataDeepChain2(v) {}
	virtual ~DeepChain2() = default;
	[[nodiscard]] bool operator==(const DeepChain2& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(DeepChain2)

struct DeepChain3 : DeepChain2 {
	mutable int usedSerializeFunction = 0;
	int dataDeepChain3 = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<DeepChain2>(this);
		ar.nvp("dataDeepChain3", dataDeepChain3);
	}
	DeepChain3() = default;
	explicit DeepChain3(IV v) :
		DeepChain2(v),
		dataDeepChain3(v) {}
	virtual ~DeepChain3() = default;
	[[nodiscard]] bool operator==(const DeepChain3& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(DeepChain3)

struct DeepChain4 : DeepChain3 {
	mutable int usedSerializeFunction = 0;
	int dataDeepChain4 = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<DeepChain3>(this);
		ar.nvp("dataDeepChain4", dataDeepChain4);
	}
	DeepChain4() = default;
	explicit DeepChain4(IV v) :
		DeepChain3(v),
		dataDeepChain4(v) {}
	virtual ~DeepChain4() = default;
	[[nodiscard]] bool operator==(const DeepChain4& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(DeepChain4)

struct DeepChain5 : DeepChain4 {
	mutable int usedSerializeFunction = 0;
	int dataDeepChain5 = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<DeepChain4>(this);
		ar.nvp("dataDeepChain5", dataDeepChain5);
	}
	DeepChain5() = default;
	explicit DeepChain5(IV v) :
		DeepChain4(v),
		dataDeepChain5(v) {}
	virtual ~DeepChain5() = default;
	[[nodiscard]] bool operator==(const DeepChain5& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(DeepChain5)

// -------------------------------------------------------------------------------------------------

struct DeepVirtualChain3 : virtual SingleVirtualBaseA_SimpleA {
	mutable int usedSerializeFunction = 0;
	int dataDeepVirtualChain3 = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template virtual_base_class<SingleVirtualBaseA_SimpleA>(this);
		ar.nvp("dataDeepVirtualChain3", dataDeepVirtualChain3);
	}
	DeepVirtualChain3() = default;
	explicit DeepVirtualChain3(IV v) :
		RootSimpleA(v),
		SingleVirtualBaseA_SimpleA(v),
		dataDeepVirtualChain3(v) {}
	virtual ~DeepVirtualChain3() = default;
	[[nodiscard]] bool operator==(const DeepVirtualChain3& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(DeepVirtualChain3)

struct DeepVirtualChain4 : virtual DeepVirtualChain3 {
	mutable int usedSerializeFunction = 0;
	int dataDeepVirtualChain4 = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template virtual_base_class<DeepVirtualChain3>(this);
		ar.nvp("dataDeepVirtualChain4", dataDeepVirtualChain4);
	}
	DeepVirtualChain4() = default;
	explicit DeepVirtualChain4(IV v) :
		RootSimpleA(v),
		SingleVirtualBaseA_SimpleA(v),
		DeepVirtualChain3(v),
		dataDeepVirtualChain4(v) {}
	virtual ~DeepVirtualChain4() = default;
	[[nodiscard]] bool operator==(const DeepVirtualChain4& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(DeepVirtualChain4)

struct DeepVirtualChain5 : virtual DeepVirtualChain4 {
	mutable int usedSerializeFunction = 0;
	int dataDeepVirtualChain5 = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template virtual_base_class<DeepVirtualChain4>(this);
		ar.nvp("dataDeepVirtualChain5", dataDeepVirtualChain5);
	}
	DeepVirtualChain5() = default;
	explicit DeepVirtualChain5(IV v) :
		RootSimpleA(v),
		SingleVirtualBaseA_SimpleA(v),
		DeepVirtualChain3(v),
		DeepVirtualChain4(v),
		dataDeepVirtualChain5(v) {}
	virtual ~DeepVirtualChain5() = default;
	[[nodiscard]] bool operator==(const DeepVirtualChain5& other) const noexcept = default;
};
VIDE_REGISTER_TYPE(DeepVirtualChain5)

// -------------------------------------------------------------------------------------------------

// Everything: offset, multi-virtual, non-virtual, diamond, duplicate
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winaccessible-base"
struct StressAggregate : PreamblePadding, virtual LeafNonFinalA_SimpleA, virtual SingleVirtualBaseA_SimpleA, LeafNonFinalB_SimpleA, SingleVirtualBaseB_SimpleA {
	mutable int usedSerializeFunction = 0;
	int dataStressAggregate = 0;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template virtual_base_class<LeafNonFinalA_SimpleA>(this);
		ar.template virtual_base_class<SingleVirtualBaseA_SimpleA>(this);
		ar.template base_class<LeafNonFinalB_SimpleA>(this);
		ar.template base_class<SingleVirtualBaseB_SimpleA>(this);
		ar.nvp("dataStressAggregate", dataStressAggregate);
	}
	StressAggregate() = default;
	explicit StressAggregate(IV v) :
		LeafNonFinalA_SimpleA(v),
		RootSimpleA(v),
		SingleVirtualBaseA_SimpleA(v),
		LeafNonFinalB_SimpleA(v),
		SingleVirtualBaseB_SimpleA(v),
		dataStressAggregate(v) {}
	virtual ~StressAggregate() = default;
	[[nodiscard]] bool operator==(const StressAggregate& other) const noexcept = default;
};
#pragma GCC diagnostic pop
VIDE_REGISTER_TYPE(StressAggregate)

// -------------------------------------------------------------------------------------------------

struct WeakHolderBase;
struct WeakHolderDerived;

struct WeakHolderBase {
	mutable int usedSerializeFunction = 0;
	int dataWeakHolderBase = 0;
	std::weak_ptr<WeakHolderBase> ptrWeakHolderBase_Base;
	std::weak_ptr<WeakHolderDerived> ptrWeakHolderBase_Derived;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.nvp("dataWeakHolderBase", dataWeakHolderBase);
		ar.nvp("ptrWeakHolderBase_Base", ptrWeakHolderBase_Base);
		ar.nvp("ptrWeakHolderBase_Derived", ptrWeakHolderBase_Derived);
	}
	WeakHolderBase() = default;
	explicit WeakHolderBase(IV v) :
		dataWeakHolderBase(v) {}
	virtual ~WeakHolderBase() = default;
	[[nodiscard]] bool operator==(const WeakHolderBase& other) const noexcept {
		return
				usedSerializeFunction == other.usedSerializeFunction &&
				dataWeakHolderBase == other.dataWeakHolderBase;
	}
};
VIDE_REGISTER_TYPE(WeakHolderBase)

struct WeakHolderDerived : WeakHolderBase {
	mutable int usedSerializeFunction = 0;
	int dataWeakHolderDerived = 0;
	std::weak_ptr<WeakHolderBase> ptrWeakHolderDerived_Base;
	std::weak_ptr<WeakHolderDerived> ptrWeakHolderDerived_Derived;
	template <typename Archive> void serialize(Archive& ar) {
		usedSerializeFunction = usedSerializeFunction == 0 ? __LINE__ : std::min(usedSerializeFunction, 0) - 1;
		ar.template base_class<WeakHolderBase>(this);
		ar.nvp("dataWeakHolderDerived", dataWeakHolderDerived);
		ar.nvp("ptrWeakHolderDerived_Base", ptrWeakHolderDerived_Base);
		ar.nvp("ptrWeakHolderDerived_Derived", ptrWeakHolderDerived_Derived);
	}
	WeakHolderDerived() = default;
	explicit WeakHolderDerived(IV v) :
		WeakHolderBase(v),
		dataWeakHolderDerived(v) {}
	virtual ~WeakHolderDerived() = default;
	[[nodiscard]] bool operator==(const WeakHolderDerived& other) const noexcept {
		return
				static_cast<const WeakHolderBase&>(*this) == static_cast<const WeakHolderBase&>(other) &&
				usedSerializeFunction == other.usedSerializeFunction &&
				dataWeakHolderDerived == other.dataWeakHolderDerived;
	}
};
VIDE_REGISTER_TYPE(WeakHolderDerived)

// =================================================================================================

// TODO P2: This is an amazing concept, move it into a common header and use it
template <class IArchive, class OArchive>
struct Tester {
	struct BaseTask {
		std::source_location testLocation;

		explicit BaseTask(std::source_location&& testLocation) : testLocation(std::move(testLocation)) {}

		virtual void out(OArchive&) = 0;
		virtual void in(IArchive&) = 0;
		virtual void check() = 0;
		virtual ~BaseTask() = default;

		std::string formatExceptionMessage(std::string_view operation, const char* exceptionType, const char* originalMessage) const {
			std::ostringstream os;
			os << "\n";
			os << "\tIn test task: " << testLocation.file_name() << ":" << testLocation.line() << ":" << testLocation.column() << "\n";
			os << "\tDuring " << operation << " test task THROW exception of type [" << vide::util::demangle(exceptionType) << "] with message:\n";
			os << "\t\t" << originalMessage;
			return std::move(os).str();
		}
	};

	std::vector<std::unique_ptr<BaseTask>> tasks;

	template <typename T>
	using CheckFn = std::function<void(const std::remove_cvref_t<T>&, const std::remove_cvref_t<T>&)>;

	template <typename T>
	void operator()(T&& output, CheckFn<T> checkFn, std::source_location testLocation = std::source_location::current()) {
		struct Task : BaseTask {
			std::remove_cvref_t<T> output;
			std::remove_cvref_t<T> input;
			CheckFn<T> checkFn;

			explicit Task(T&& output, CheckFn<T>&& checkFn, std::source_location&& testLocation) :
				BaseTask{std::move(testLocation)},
				output(std::forward<T>(output)),
				checkFn(std::move(checkFn)) {
			}

			virtual void out(OArchive& oar) override {
				try {
					oar(std::as_const(output));
				} catch (const std::exception& e) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("output serialization", typeid(e).name(), e.what()));
				} catch (...) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("output serialization", "unhandled exception", "?"));
				}
			}
			virtual void in(IArchive& iar) override {
				try {
					iar(input);
				} catch (const std::exception& e) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("input serialization", typeid(e).name(), e.what()));
				} catch (...) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("input serialization", "unhandled exception", "?"));
				}
			}
			virtual void check() override {
				try {
					checkFn(std::as_const(output), std::as_const(input));
				} catch (const std::exception& e) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("result evaluation", typeid(e).name(), e.what()));
				} catch (...) {
					throw std::runtime_error(BaseTask::formatExceptionMessage("result evaluation", "unhandled exception", "?"));
				}
			}
		};

		tasks.emplace_back(std::make_unique<Task>(std::forward<T>(output), std::move(checkFn), std::move(testLocation)));
	}

	void outputAll(OArchive& oar) {
		for (const auto& task : tasks)
			task->out(oar);
	}
	void inputAll(IArchive& iar) {
		for (const auto& task : tasks)
			task->in(iar);
	}
	void checkAll() {
		for (const auto& task : tasks)
			task->check();
	}
	void shuffle(std::mt19937& rng) {
		std::ranges::shuffle(tasks, rng);
	}
};

template <typename T>
[[nodiscard]] std::shared_ptr<T> shared(std::nullptr_t) {
	return std::shared_ptr<T>(nullptr);
}

template <typename T, typename... Args>
[[nodiscard]] std::shared_ptr<T> shared(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename Derived, typename Base, typename... Args>
[[nodiscard]] std::shared_ptr<Base> shared_as(Args&&... args) {
	return std::static_pointer_cast<Base>(std::make_shared<Derived>(std::forward<Args>(args)...));
}

template <typename To, typename From>
[[nodiscard]] std::shared_ptr<To> dcast(const std::shared_ptr<From>& ptr) {
	return std::dynamic_pointer_cast<To>(ptr);
}

template <typename To, typename From>
[[nodiscard]] std::shared_ptr<To> scast(const std::shared_ptr<From>& ptr) {
	return std::static_pointer_cast<To>(ptr);
}

template <typename To, typename From>
[[nodiscard]] bool castable_and_equal(const std::shared_ptr<From>& in, const std::shared_ptr<From>& out) {
	const auto o = std::dynamic_pointer_cast<To>(out);
	const auto i = std::dynamic_pointer_cast<To>(in);
	return o && i && *i == *o;
}

// =================================================================================================

template <class IArchive, class OArchive>
void test_polymorphic() {
// template <class IArchive2, class OArchive2>
// void test_polymorphic() {
// 	using IArchive = vide::underlying_archive_t<IArchive2>;
// 	using OArchive = vide::underlying_archive_t<OArchive2>;
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	// auto rngB = [&] { return random_value<int>(rng) % 2 == 0; };
	// auto rngI = [&] { return random_value<int>(rng); };
	// auto rngL = [&] { return random_value<long>(rng); };
	// auto rngF = [&] { return random_value<float>(rng); };
	// auto rngD = [&] { return random_value<double>(rng); };

	for (int i = 0; i < 100; ++i) {
		Tester<IArchive, OArchive> test;

		// --- RootRegularEmpty* ---

		test(shared<RootRegularEmptyA>(nullptr), [](const auto&, const auto& in) {
			CHECK_EQ(in.get(), nullptr);
		});

		test(shared<RootRegularEmptyA>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootRegularEmptyB>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootRegularEmptyC>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		// --- RootRegularSimple* ---

		test(shared<RootRegularSimpleA>(nullptr), [](const auto&, const auto& in) {
			CHECK_EQ(in.get(), nullptr);
		});
		test(shared<RootRegularSimpleA>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootRegularSimpleB>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootRegularSimpleC>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		// --- RootEmpty* ---

		test(shared<RootEmptyA>(nullptr), [](const auto&, const auto& in) {
			CHECK_EQ(in.get(), nullptr);
		});

		test(shared<RootEmptyA>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootEmptyB>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootEmptyC>(), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		// --- RootSimple* ---

		test(shared<RootSimpleA>(nullptr), [](const auto&, const auto& in) {
			CHECK_EQ(in.get(), nullptr);
		});
		test(shared<RootSimpleA>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootSimpleB>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		test(shared<RootSimpleC>(init), [](const auto& out, const auto& in) {
			REQUIRE_NE(in.get(), nullptr);
			CHECK_NE(in.get(), out.get());
			CHECK_EQ(*in, *out);
		});

		// --- LeafFinal_* ---

		test(shared<LeafFinal_EmptyA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});

		test(shared<LeafFinal_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});

		test(shared_as<LeafFinal_EmptyA, RootEmptyA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeafFinal_EmptyA>(in, out));
		});

		test(shared_as<LeafFinal_SimpleA, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeafFinal_SimpleA>(in, out));
		});

		// --- LeafNonFinal_* ---

		test(shared<LeafNonFinal_EmptyA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});

		test(shared<LeafNonFinalA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});

		test(shared<LeafNonFinalB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});

		test(shared_as<LeafNonFinal_EmptyA, RootEmptyA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeafNonFinal_EmptyA>(in, out));
		});

		test(shared_as<LeafNonFinalA_SimpleA, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeafNonFinalA_SimpleA>(in, out));
		});

		test(shared_as<LeafNonFinalB_SimpleA, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeafNonFinalB_SimpleA>(in, out));
		});

		// --- BaseAtNonZeroOffset ---

		test(shared<BaseAtNonZeroOffset>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<BaseAtNonZeroOffset, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<BaseAtNonZeroOffset>(in, out));
		});

		// --- MultipleNonVirtualBases ---

		test(shared<MultipleNonVirtualBases>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<MultipleNonVirtualBases, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleNonVirtualBases>(in, out));
		});
		test(shared_as<MultipleNonVirtualBases, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleNonVirtualBases>(in, out));
		});

		test(shared<MultipleNonVirtualReverseBases>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<MultipleNonVirtualReverseBases, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleNonVirtualReverseBases>(in, out));
		});
		test(shared_as<MultipleNonVirtualReverseBases, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleNonVirtualReverseBases>(in, out));
		});

		// --- SingleVirtualBase* ---

		test(shared<SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<SingleVirtualBaseA_SimpleA, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<SingleVirtualBaseA_SimpleA>(in, out));
		});

		test(shared<SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<SingleVirtualBaseB_SimpleA, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<SingleVirtualBaseB_SimpleA>(in, out));
		});

		// --- MultipleIndependentVirtualBase ---

		test(shared<MultipleIndependentVirtualBase>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<MultipleIndependentVirtualBase, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleIndependentVirtualBase>(in, out));
		});
		test(shared_as<MultipleIndependentVirtualBase, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleIndependentVirtualBase>(in, out));
		});
		test(shared_as<MultipleIndependentVirtualBase, RootSimpleC>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MultipleIndependentVirtualBase>(in, out));
		});

		// --- *VirtualBaseAfter*VirtualBase ---

		test(shared<VirtualBaseAfterNonVirtualBase>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<VirtualBaseAfterNonVirtualBase, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<VirtualBaseAfterNonVirtualBase>(in, out));
		});
		test(shared_as<VirtualBaseAfterNonVirtualBase, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<VirtualBaseAfterNonVirtualBase>(in, out));
		});

		test(shared<NonVirtualBaseAfterVirtualBase>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<NonVirtualBaseAfterVirtualBase, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<NonVirtualBaseAfterVirtualBase>(in, out));
		});
		test(shared_as<NonVirtualBaseAfterVirtualBase, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<NonVirtualBaseAfterVirtualBase>(in, out));
		});

		// --- *VirtualDiamond ---

		test(shared<ClassicVirtualDiamond>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<ClassicVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<ClassicVirtualDiamond>(in, out));
		});
		test(shared_as<ClassicVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<ClassicVirtualDiamond>(in, out));
		});
		test(shared_as<ClassicVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<ClassicVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		test(shared<LeftVirtualVirtualDiamond>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<LeftVirtualVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeftVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<LeftVirtualVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeftVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<LeftVirtualVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<LeftVirtualVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		test(shared<RightVirtualVirtualDiamond>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<RightVirtualVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<RightVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<RightVirtualVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<RightVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<RightVirtualVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<RightVirtualVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		test(shared<BothVirtualVirtualDiamond>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<BothVirtualVirtualDiamond, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<BothVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<BothVirtualVirtualDiamond, SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<BothVirtualVirtualDiamond>(in, out));
		});
		test(shared_as<BothVirtualVirtualDiamond, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<BothVirtualVirtualDiamond>(in, out));
			CHECK(dcast<SingleVirtualBaseA_SimpleA>(in)); // down cast
			CHECK(dcast<SingleVirtualBaseB_SimpleA>(in)); // down cast
		});

		// --- MixedDepth ---

		test(shared<MixedDepth>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<MixedDepth, LeafNonFinalA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MixedDepth>(in, out));
		});
		test(shared_as<MixedDepth, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MixedDepth>(in, out));
		});
		test(shared_as<MixedDepth, RootSimpleB>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<MixedDepth>(in, out));
		});

		// --- DuplicateNonVirtualBase ---

		test(shared<DuplicateNonVirtualBase>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<DuplicateNonVirtualBase, LeafNonFinalA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DuplicateNonVirtualBase>(in, out));
		});
		test(shared_as<DuplicateNonVirtualBase, LeafNonFinalB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DuplicateNonVirtualBase>(in, out));
		});
		test(scast<RootSimpleA>(shared_as<DuplicateNonVirtualBase, LeafNonFinalA_SimpleA>(init)), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DuplicateNonVirtualBase>(in, out));
		});
		test(scast<RootSimpleA>(shared_as<DuplicateNonVirtualBase, LeafNonFinalB_SimpleA>(init)), [](const auto& out, const auto& in) {
			// BUG: DuplicateNonVirtualBase downcast incorrectly selects the first child instead of what was pointed to
			// So this is a funny setup, and definitely a bug. When we save pointing to RootSimpleA #2 and loading
			// it back with RootSimpleA incorrectly RootSimple #1 will be selected.
			// To fix this we need have to detect duplicate types in the hierarchy and also serialize an index during
			// downcast and use that index during upcast.
			//
			//           Input ->  RootSimple #1    RootSimpleA #2  <- Output
			//                           ^                ^
			//                           |                |
			//                     LeafNonFinalA    LeafNonFinalB
			//                            ^              ^
			//                             \            /
			//                        DuplicateNonVirtualBase

			// CHECK_EQ(*in, *out); This should pass
			CHECK_EQ(*scast<RootSimpleA>(dcast<LeafNonFinalB_SimpleA>(in)), *out); // Current status quo
			CHECK(castable_and_equal<DuplicateNonVirtualBase>(in, out));
		});

		// --- DeepChain5 ---

		test(shared<DeepChain5>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<DeepChain5, DeepChain1>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepChain5>(in, out));
		});
		test(shared_as<DeepChain5, DeepChain2>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepChain5>(in, out));
		});
		test(shared_as<DeepChain5, DeepChain3>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepChain5>(in, out));
		});
		test(shared_as<DeepChain5, DeepChain4>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepChain5>(in, out));
		});

		// --- DeepVirtualChain5 ---

		test(shared<DeepVirtualChain5>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<DeepVirtualChain5, RootSimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepVirtualChain5>(in, out));
		});
		test(shared_as<DeepVirtualChain5, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepVirtualChain5>(in, out));
		});
		test(shared_as<DeepVirtualChain5, DeepVirtualChain3>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepVirtualChain5>(in, out));
		});
		test(shared_as<DeepVirtualChain5, DeepVirtualChain4>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<DeepVirtualChain5>(in, out));
		});

		// --- StressAggregate ---

		test(shared<StressAggregate>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
		});
		test(shared_as<StressAggregate, LeafNonFinalA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});
		test(shared_as<StressAggregate, SingleVirtualBaseA_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});
		test(shared_as<StressAggregate, LeafNonFinalB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});
		test(shared_as<StressAggregate, SingleVirtualBaseB_SimpleA>(init), [](const auto& out, const auto& in) {
			CHECK_EQ(*in, *out);
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});
		test(scast<RootSimpleA>(shared_as<StressAggregate, SingleVirtualBaseA_SimpleA>(init)), [](const auto& out, const auto& in) {
			// BUG: DuplicateNonVirtualBase downcast incorrectly (see DuplicateNonVirtualBase)
			// CHECK_EQ(*in, *out); This should pass
			CHECK_EQ(*scast<RootSimpleA>(dcast<SingleVirtualBaseA_SimpleA>(in)), *out); // Current status quo
			CHECK(castable_and_equal<StressAggregate>(in, out));
		});

		// --- Self References ---

		{
			// A -> A
			const auto a = shared<WeakHolderBase>(init);
			a->ptrWeakHolderBase_Base = a;
			test(a, [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK_EQ(in->ptrWeakHolderBase_Base.lock(), in);
			});
		} {
			// WeakHolderDerived: A base -> A base
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderBase_Base = a;
			test(a, [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK_EQ(in->ptrWeakHolderBase_Base.lock(), in);
			});
		} {
			// WeakHolderDerived: A derived -> A base
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderDerived_Base = a;
			test(a, [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK_EQ(in->ptrWeakHolderDerived_Base.lock(), in);
			});
		} {
			// WeakHolderDerived: A base -> A derived
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderBase_Derived = a;
			test(a, [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK_EQ(in->ptrWeakHolderBase_Derived.lock(), in);
			});
		} {
			// WeakHolderDerived: A derived -> A derived
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderDerived_Derived = a;
			test(a, [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK_EQ(in->ptrWeakHolderDerived_Derived.lock(), in);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A base -> A base
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderBase_Base = a;
			test(scast<WeakHolderBase>(a), [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK(castable_and_equal<WeakHolderDerived>(in, out));
				CHECK_EQ(in->ptrWeakHolderBase_Base.lock(), in);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A derived -> A base
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderDerived_Base = a;
			test(scast<WeakHolderBase>(a), [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK(castable_and_equal<WeakHolderDerived>(in, out));
				const auto derivedIn = dcast<WeakHolderDerived>(in);
				CHECK((derivedIn && derivedIn->ptrWeakHolderDerived_Base.lock() == derivedIn));
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A base -> A derived
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderBase_Derived = a;
			test(scast<WeakHolderBase>(a), [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK(castable_and_equal<WeakHolderDerived>(in, out));
				CHECK_EQ(in->ptrWeakHolderBase_Derived.lock(), in);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A derived -> A derived
			const auto a = shared<WeakHolderDerived>(init);
			a->ptrWeakHolderDerived_Derived = a;
			test(scast<WeakHolderBase>(a), [](const auto& out, const auto& in) {
				CHECK_EQ(*in, *out);
				CHECK(castable_and_equal<WeakHolderDerived>(in, out));
				const auto derivedIn = dcast<WeakHolderDerived>(in);
				CHECK((derivedIn && derivedIn->ptrWeakHolderDerived_Derived.lock() == derivedIn));
			});
		}

		// --- Cyclic References ---

		{
			// A -> B
			// B -> A
			struct _ {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			var.a = shared<WeakHolderBase>(init);
			var.b = shared<WeakHolderBase>(init);
			var.a->ptrWeakHolderBase_Base = var.b;
			var.b->ptrWeakHolderBase_Base = var.a;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(in.a->ptrWeakHolderBase_Base.lock(), in.b);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(in.b->ptrWeakHolderBase_Base.lock(), in.a);
			});
		} {
			// A -> B
			// B -> C
			// C -> A
			struct _ {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				std::shared_ptr<WeakHolderBase> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			var.a = shared<WeakHolderBase>(init);
			var.b = shared<WeakHolderBase>(init);
			var.c = shared<WeakHolderBase>(init);
			var.a->ptrWeakHolderBase_Base = var.b;
			var.b->ptrWeakHolderBase_Base = var.c;
			var.c->ptrWeakHolderBase_Base = var.a;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(in.a->ptrWeakHolderBase_Base.lock(), in.b);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(in.b->ptrWeakHolderBase_Base.lock(), in.c);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(in.c->ptrWeakHolderBase_Base.lock(), in.a);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A base -> B base
			// WeakHolderDerived as WeakHolderBase: B base -> A base
			struct _ {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			var.a = shared<WeakHolderDerived>(init);
			var.b = shared<WeakHolderDerived>(init);
			scast<WeakHolderDerived>(var.a)->ptrWeakHolderBase_Base = var.b;
			scast<WeakHolderDerived>(var.b)->ptrWeakHolderBase_Base = var.a;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK_EQ(in.a->ptrWeakHolderBase_Base.lock(), in.b);
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<WeakHolderDerived>(in.b, out.b));
				CHECK_EQ(in.b->ptrWeakHolderBase_Base.lock(), in.a);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A base -> B base
			// WeakHolderDerived as WeakHolderBase: B base -> C base
			// WeakHolderDerived as WeakHolderBase: C base -> A base
			struct _ {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				std::shared_ptr<WeakHolderBase> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			var.a = shared<WeakHolderDerived>(init);
			var.b = shared<WeakHolderDerived>(init);
			var.c = shared<WeakHolderDerived>(init);
			scast<WeakHolderDerived>(var.a)->ptrWeakHolderBase_Base = var.b;
			scast<WeakHolderDerived>(var.b)->ptrWeakHolderBase_Base = var.c;
			scast<WeakHolderDerived>(var.c)->ptrWeakHolderBase_Base = var.a;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK_EQ(in.a->ptrWeakHolderBase_Base.lock(), in.b);
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<WeakHolderDerived>(in.b, out.b));
				CHECK_EQ(in.b->ptrWeakHolderBase_Base.lock(), in.c);
				CHECK_EQ(*in.c, *out.c);
				CHECK(castable_and_equal<WeakHolderDerived>(in.c, out.c));
				CHECK_EQ(in.c->ptrWeakHolderBase_Base.lock(), in.a);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A base -> B derived
			// WeakHolderDerived as WeakHolderBase: B base -> A derived
			struct _ {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			var.a = shared<WeakHolderDerived>(init);
			var.b = shared<WeakHolderDerived>(init);
			scast<WeakHolderDerived>(var.a)->ptrWeakHolderBase_Derived = scast<WeakHolderDerived>(var.b);
			scast<WeakHolderDerived>(var.b)->ptrWeakHolderBase_Derived = scast<WeakHolderDerived>(var.a);
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK_EQ(in.a->ptrWeakHolderBase_Derived.lock(), in.b);
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<WeakHolderDerived>(in.b, out.b));
				CHECK_EQ(in.b->ptrWeakHolderBase_Derived.lock(), in.a);
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A derived -> B derived
			// WeakHolderDerived as WeakHolderBase: B derived -> A derived
			struct _ {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			var.a = shared<WeakHolderDerived>(init);
			var.b = shared<WeakHolderDerived>(init);
			scast<WeakHolderDerived>(var.a)->ptrWeakHolderDerived_Derived = scast<WeakHolderDerived>(var.b);
			scast<WeakHolderDerived>(var.b)->ptrWeakHolderDerived_Derived = scast<WeakHolderDerived>(var.a);
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				const auto derivedInA = dcast<WeakHolderDerived>(in.a);
				const auto derivedInB = dcast<WeakHolderDerived>(in.b);
				CHECK_EQ(*in.a, *out.a);
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK((derivedInA && derivedInA->ptrWeakHolderDerived_Derived.lock() == derivedInB));
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<WeakHolderDerived>(in.b, out.b));
				CHECK((derivedInB && derivedInB->ptrWeakHolderDerived_Derived.lock() == derivedInA));
			});
		} {
			// WeakHolderDerived as WeakHolderBase: A derived -> B derived
			// WeakHolderDerived as WeakHolderBase: B derived -> C derived
			// WeakHolderDerived as WeakHolderBase: C derived -> A derived
			struct _ {
				std::shared_ptr<WeakHolderBase> a;
				std::shared_ptr<WeakHolderBase> b;
				std::shared_ptr<WeakHolderBase> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			var.a = shared<WeakHolderDerived>(init);
			var.b = shared<WeakHolderDerived>(init);
			var.c = shared<WeakHolderDerived>(init);
			scast<WeakHolderDerived>(var.a)->ptrWeakHolderDerived_Derived = scast<WeakHolderDerived>(var.b);
			scast<WeakHolderDerived>(var.b)->ptrWeakHolderDerived_Derived = scast<WeakHolderDerived>(var.c);
			scast<WeakHolderDerived>(var.c)->ptrWeakHolderDerived_Derived = scast<WeakHolderDerived>(var.a);
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				const auto derivedInA = dcast<WeakHolderDerived>(in.a);
				const auto derivedInB = dcast<WeakHolderDerived>(in.b);
				const auto derivedInC = dcast<WeakHolderDerived>(in.c);
				CHECK_EQ(*in.a, *out.a);
				CHECK(castable_and_equal<WeakHolderDerived>(in.a, out.a));
				CHECK((derivedInA && derivedInA->ptrWeakHolderDerived_Derived.lock() == derivedInB));
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<WeakHolderDerived>(in.b, out.b));
				CHECK((derivedInB && derivedInB->ptrWeakHolderDerived_Derived.lock() == derivedInC));
				CHECK_EQ(*in.c, *out.c);
				CHECK(castable_and_equal<WeakHolderDerived>(in.c, out.c));
				CHECK((derivedInC && derivedInC->ptrWeakHolderDerived_Derived.lock() == derivedInA));
			});
		}

		// --- Polymorphic loading via different types to the same object ---

		// Deep chain
		{
			struct _ {
				std::shared_ptr<DeepChain5> a;
				std::shared_ptr<DeepChain4> b;
				std::shared_ptr<DeepChain3> c;
				std::shared_ptr<DeepChain2> d;
				std::shared_ptr<DeepChain1> e;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d)(e); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d)(e); }
			} var;

			auto p = shared<DeepChain5>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			var.e = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK_EQ(*in.e, *out.e);
				CHECK(castable_and_equal<DeepChain5>(in.a, out.a));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.b));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.c));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.d));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.e));
			});
		} {
			struct _ {
				std::shared_ptr<DeepChain1> a;
				std::shared_ptr<DeepChain2> b;
				std::shared_ptr<DeepChain3> c;
				std::shared_ptr<DeepChain4> d;
				std::shared_ptr<DeepChain5> e;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d)(e); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d)(e); }
			} var;

			auto p = shared<DeepChain5>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			var.e = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK_EQ(*in.e, *out.e);
				CHECK(castable_and_equal<DeepChain5>(in.a, out.a));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.b));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.c));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.d));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.e));
			});
		} {
			struct _ {
				std::shared_ptr<DeepChain3> a;
				std::shared_ptr<DeepChain2> b;
				std::shared_ptr<DeepChain1> c;
				std::shared_ptr<DeepChain5> d;
				std::shared_ptr<DeepChain4> e;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d)(e); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d)(e); }
			} var;

			auto p = shared<DeepChain5>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			var.e = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK_EQ(*in.e, *out.e);
				CHECK(castable_and_equal<DeepChain5>(in.a, out.a));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.b));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.c));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.d));
				CHECK_EQ(dcast<DeepChain5>(in.a), dcast<DeepChain5>(in.e));
			});
		}

		{ // BaseAtNonZeroOffset
			struct _ {
				std::shared_ptr<BaseAtNonZeroOffset> a;
				std::shared_ptr<RootSimpleA> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			auto p = shared<BaseAtNonZeroOffset>(init);
			var.a = p;
			var.b = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<BaseAtNonZeroOffset>(in.a, out.a));
				CHECK_EQ(dcast<BaseAtNonZeroOffset>(in.a), dcast<BaseAtNonZeroOffset>(in.b));
			});
		} {
			struct _ {
				std::shared_ptr<RootSimpleA> a;
				std::shared_ptr<BaseAtNonZeroOffset> b;
				void serialize(IArchive& ar) { ar(a)(b); }
				void serialize(OArchive& ar) { ar(a)(b); }
			} var;

			auto p = shared<BaseAtNonZeroOffset>(init);
			var.a = p;
			var.b = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK(castable_and_equal<BaseAtNonZeroOffset>(in.a, out.a));
				CHECK_EQ(dcast<BaseAtNonZeroOffset>(in.a), dcast<BaseAtNonZeroOffset>(in.b));
			});
		}

		// ClassicVirtualDiamond
		{
			struct _ {
				std::shared_ptr<ClassicVirtualDiamond> a;
				std::shared_ptr<SingleVirtualBaseA_SimpleA> b;
				std::shared_ptr<SingleVirtualBaseB_SimpleA> c;
				std::shared_ptr<RootSimpleA> d;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d); }
			} var;

			auto p = shared<ClassicVirtualDiamond>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK(castable_and_equal<ClassicVirtualDiamond>(in.a, out.a));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.b));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.c));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.d));
			});
		} {
			struct _ {
				std::shared_ptr<RootSimpleA> a;
				std::shared_ptr<SingleVirtualBaseA_SimpleA> b;
				std::shared_ptr<SingleVirtualBaseB_SimpleA> c;
				std::shared_ptr<ClassicVirtualDiamond> d;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d); }
			} var;

			auto p = shared<ClassicVirtualDiamond>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK(castable_and_equal<ClassicVirtualDiamond>(in.a, out.a));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.b));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.c));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.d));
			});
		} {
			struct _ {
				std::shared_ptr<SingleVirtualBaseB_SimpleA> a;
				std::shared_ptr<RootSimpleA> b;
				std::shared_ptr<ClassicVirtualDiamond> c;
				std::shared_ptr<SingleVirtualBaseA_SimpleA> d;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d); }
			} var;

			auto p = shared<ClassicVirtualDiamond>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK(castable_and_equal<ClassicVirtualDiamond>(in.a, out.a));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.b));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.c));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.d));
			});
		} {
			struct _ {
				std::shared_ptr<SingleVirtualBaseA_SimpleA> a;
				std::shared_ptr<RootSimpleA> b;
				std::shared_ptr<SingleVirtualBaseB_SimpleA> c;
				std::shared_ptr<ClassicVirtualDiamond> d;
				void serialize(IArchive& ar) { ar(a)(b)(c)(d); }
				void serialize(OArchive& ar) { ar(a)(b)(c)(d); }
			} var;

			auto p = shared<ClassicVirtualDiamond>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			var.d = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK_EQ(*in.d, *out.d);
				CHECK(castable_and_equal<ClassicVirtualDiamond>(in.a, out.a));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.b));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.c));
				CHECK_EQ(dcast<ClassicVirtualDiamond>(in.a), dcast<ClassicVirtualDiamond>(in.d));
			});
		}

		{ // MultipleNonVirtualBases
			struct _ {
				std::shared_ptr<MultipleNonVirtualBases> a;
				std::shared_ptr<RootSimpleA> b;
				std::shared_ptr<RootSimpleB> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			auto p = shared<MultipleNonVirtualBases>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK(castable_and_equal<MultipleNonVirtualBases>(in.a, out.a));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.b));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.c));
			});
		} {
			struct _ {
				std::shared_ptr<RootSimpleA> a;
				std::shared_ptr<MultipleNonVirtualBases> b;
				std::shared_ptr<RootSimpleB> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			auto p = shared<MultipleNonVirtualBases>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK(castable_and_equal<MultipleNonVirtualBases>(in.a, out.a));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.b));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.c));
			});
		} {
			struct _ {
				std::shared_ptr<RootSimpleB> a;
				std::shared_ptr<RootSimpleA> b;
				std::shared_ptr<MultipleNonVirtualBases> c;
				void serialize(IArchive& ar) { ar(a)(b)(c); }
				void serialize(OArchive& ar) { ar(a)(b)(c); }
			} var;

			auto p = shared<MultipleNonVirtualBases>(init);
			var.a = p;
			var.b = p;
			var.c = p;
			test(var, [](const decltype(var)& out, const decltype(var)& in) {
				CHECK_EQ(*in.a, *out.a);
				CHECK_EQ(*in.b, *out.b);
				CHECK_EQ(*in.c, *out.c);
				CHECK(castable_and_equal<MultipleNonVirtualBases>(in.a, out.a));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.b));
				CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.c));
			});
		}

		// // --- Incorrect type deserialization
		//
		// {
		// 	struct _ {
		// 		std::shared_ptr<RootSimpleB> a;
		// 		std::shared_ptr<RootSimpleA> b;
		// 		std::shared_ptr<MultipleNonVirtualBases> c;
		// 		void serialize(IArchive& ar) { ar(a)(b)(c); }
		// 		void serialize(OArchive& ar) { ar(a)(b)(c); }
		// 	} var;
		//
		// 	auto p = shared<MultipleNonVirtualBases>(init);
		// 	var.a = p;
		// 	var.b = p;
		// 	var.c = p;
		// 	test(var, [](const decltype(var)& out, const decltype(var)& in) {
		// 		CHECK_EQ(*in.a, *out.a);
		// 		CHECK_EQ(*in.b, *out.b);
		// 		CHECK_EQ(*in.c, *out.c);
		// 		CHECK(castable_and_equal<MultipleNonVirtualBases>(in.a, out.a));
		// 		CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.b));
		// 		CHECK_EQ(dcast<MultipleNonVirtualBases>(in.a), dcast<MultipleNonVirtualBases>(in.c));
		// 	});
		// }

		// === Runner code ===

		if (i != 0 && i != 99) // First and last is not shuffled for ease of debugging
			test.shuffle(rng);

		std::ostringstream os; {
			OArchive oar(os);
			test.outputAll(oar);
		}

		// std::cout << std::endl;
		// std::cout << "========" << std::endl;
		// std::cout << os.str() << std::endl;

		std::istringstream is(os.str()); {
			IArchive iar(is);
			test.inputAll(iar);
		}

		test.checkAll();
	}
}

template <class IArchive, class OArchive>
inline void test_polymorphic_shared_ptr_type_mismatch() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	auto o_ptr0 = std::static_pointer_cast<RootSimpleA>(std::make_shared<LeafNonFinalA_SimpleA>(init));
	auto o_ptr1 = o_ptr0;
	std::shared_ptr<RootSimpleA> i_ptr0;
	std::shared_ptr<RootSimpleB> i_ptr1; //< This will not be the same type as the data was saved with

	std::ostringstream os; {
		OArchive oar(os);
		oar(o_ptr0);
		oar(o_ptr1);
	} {
		std::istringstream is(os.str());
		IArchive iar(is);
		iar(i_ptr0);
		CHECK_THROWS_AS(iar(i_ptr1), vide::Exception);
	}
}

template <class IArchive, class OArchive>
inline void test_polymorphic_shared_ptr_type_mismatch_base() {
	std::mt19937 rng(0x5EED);
	Initializer init{rng};

	auto o_ptr0 = std::make_shared<RootSimpleA>(init);
	auto o_ptr1 = o_ptr0;
	std::shared_ptr<RootSimpleA> i_ptr0;
	std::shared_ptr<RootSimpleB> i_ptr1; //< This will not be the same type as the data was saved with

	std::ostringstream os; {
		OArchive oar(os);
		oar(o_ptr0);
		oar(o_ptr1);
	} {
		std::istringstream is(os.str());
		IArchive iar(is);
		iar(i_ptr0);
		CHECK_THROWS_AS(iar(i_ptr1), vide::Exception);
	}
}


TEST_SUITE_BEGIN("polymorphic");

// TEST_CASE("json_polymorphic") {
// 	test_polymorphic<vide::JSONInputArchive, vide::JSONOutputArchive>();
// }
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic_legacy", legacy::test_polymorphic_legacy)
CREATE_TEST_CASES_FOR_BINARY_ARCHIVE("polymorphic", test_polymorphic)
CREATE_TEST_CASES_FOR_TEXT_ARCHIVE("polymorphic", test_polymorphic)
// Proxy archives are skipped for test_polymorphic as it takes ages to compile. A separate test will take care of it
// TODO P4: Enable for proxies too:
// CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic", test_polymorphic)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic_shared_ptr_type_mismatch", test_polymorphic_shared_ptr_type_mismatch)
CREATE_TEST_CASES_FOR_ALL_ARCHIVE("polymorphic_shared_ptr_type_mismatch_base", test_polymorphic_shared_ptr_type_mismatch_base)

#if VIDE_THREAD_SAFE
CREATE_TEST_CASES_FOR_ALL_ARCHIVE ("polymorphic_threading", legacy::test_polymorphic_threading)
#endif // VIDE_THREAD_SAFE

TEST_SUITE_END();

