//

#pragma once

#include <source_location>


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
