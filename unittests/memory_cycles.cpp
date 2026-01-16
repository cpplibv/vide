//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.hpp"


struct MemoryCycle {
	MemoryCycle() = default;

	MemoryCycle(int v) :
		value(v) {
	}

	int value;
	std::weak_ptr<MemoryCycle> ptr;

	bool operator==(MemoryCycle const& other) const {
		return value == other.value && ptr.lock() == other.ptr.lock();
	}

	template <class Archive>
	void serialize(Archive& ar) {
		ar(value)(ptr);
	}
};

std::ostream& operator<<(std::ostream& os, MemoryCycle const& s) {
	os << "[value: " << s.value << " ptr: " << s.ptr.lock() << "]";
	return os;
}

template <class IArchive, class OArchive>
inline void test_memory_cycles() {
	std::random_device rd;
	std::mt19937 gen(rd());

	for (int ii = 0; ii < 100; ++ii) {
		auto o_ptr1 = std::make_shared<MemoryCycle>(random_value<int>(gen));
		o_ptr1->ptr = o_ptr1;

		std::ostringstream os; {
			OArchive oar(os);

			oar(o_ptr1);
		}

		decltype(o_ptr1) i_ptr1;

		// std::cout << std::endl;
		// std::cout << "========" << std::endl;
		// std::cout << os.str() << std::endl;

		std::istringstream is(os.str()); {
			IArchive iar(is);

			iar(i_ptr1);
		}

		CHECK_EQ(o_ptr1->value, i_ptr1->value);
		CHECK_EQ(i_ptr1.get(), i_ptr1->ptr.lock().get());
	}
}


TEST_SUITE_BEGIN("memory_cycles");

CREATE_TEST_CASES_FOR_ALL_ARCHIVE("memory_cycles", test_memory_cycles)

TEST_SUITE_END();
