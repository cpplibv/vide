#pragma once

#include "common.hpp"

struct TestBoolOwner {
	bool value = false;
	// 3 padding byte
	std::array<bool, 4> array{false, false, false, false};
	std::vector<bool> vector{false, false, false, false};
	std::deque<bool> deque{false, false, false, false};

	TestBoolOwner() = default;
	TestBoolOwner(uint8_t raw) {
		std::memset(&value, raw, sizeof(value));
		for (auto& v : array)
			std::memset(&v, raw, sizeof(v));
		for (auto&& v : vector)
			v = raw != 0; // vector<bool> bit-compression cannot store invalid values in the vector
		for (auto& v : deque)
			std::memset(&v, raw, sizeof(v));
	}

	template <class Archive> void serialize(Archive& ar) {
		ar.nvp("value", value);
		ar.nvp("array", array);
		ar.nvp("vector", vector);
		ar.nvp("deque", deque);
	}
};

template <class IArchive, class OArchive>
inline void test_bool() {

	const TestBoolOwner oFalse{0};
	const TestBoolOwner oTrue{1};
	const TestBoolOwner oInvalid{2}; // Basically UB

	{
		std::ostringstream os;
		{
			OArchive oar(os);

			oar(oFalse);
			oar(oTrue);
		}
		std::istringstream is(os.str());
		TestBoolOwner iFalse;
		TestBoolOwner iTrue;
		{
			IArchive iar(is);
			iar(iFalse);
			iar(iTrue);
		}

		CHECK_EQ(iFalse.value, oFalse.value);
		check_collection(iFalse.array, oFalse.array);
		check_collection(iFalse.vector, oFalse.vector);
		check_collection(iFalse.deque, oFalse.deque);

		CHECK_EQ(iTrue.value, oTrue.value);
		check_collection(iTrue.array, oTrue.array);
		check_collection(iTrue.vector, oTrue.vector);
		check_collection(iTrue.deque, oTrue.deque);
	}

	// This test block would call use UB and fire an assert
	// {
	// 	std::ostringstream os;
	// 	{
	// 		OArchive oar(os);
	// 		oar(oInvalid);
	// 	}
	// 	std::istringstream is(os.str());
	// 	TestBoolOwner iInvalid;
	// 	{
	// 		IArchive iar(is);
	// 		iar(iInvalid);
	// 	}
	//
	// 	CHECK_EQ(oTrue.value, iInvalid.value);
	// 	check_collection(oTrue.array, iInvalid.array);
	// 	check_collection(oTrue.vector, iInvalid.vector);
	// 	check_collection(oTrue.deque, iInvalid.deque);
	// }

	for (int offset : {0, 1, 2, 3, 4, 9, 10, 11, 12, 17, 18, 19, 20}) {
		offset += std::is_same_v<IArchive, vide::BinaryInputArchive> ? 0 : 1;

		std::ostringstream os;
		{
			OArchive oar(os);
			oar(oFalse);
		}

		auto data = os.str();
		data[offset] = 2; // Inject invalid bool value into the data

		std::istringstream is(data);
		TestBoolOwner iInvalid;
		{
			IArchive iar(is);
			const auto message = "Invalid bool value '2' at byte " + std::to_string(offset);
			CHECK_THROWS_WITH_AS(iar(iInvalid), message.c_str(), vide::Exception);
		}
	}
}
