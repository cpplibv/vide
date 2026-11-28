#include <base.hpp>
#include <derived.hpp>

#include <vide/access.hpp>
#include <vide/base_class.hpp>
#include <vide/vide.hpp>

#include <vide/types/common.hpp>
#include <vide/types/memory.hpp>
#include <vide/types/std_array.hpp>
#include <vide/types/std_bitset.hpp>
#include <vide/types/std_chrono.hpp>
#include <vide/types/std_complex.hpp>
#include <vide/types/std_deque.hpp>
#include <vide/types/std_forward_list.hpp>
#include <vide/types/std_list.hpp>
#include <vide/types/std_map.hpp>

#include <vide/details/util.hpp>

#include <vide/details/polymorphic_impl.hpp>
#include <vide/types/polymorphic.hpp>

#include <vide/types/std_queue.hpp>
#include <vide/types/std_set.hpp>
#include <vide/types/std_stack.hpp>
#include <vide/types/std_string.hpp>
#include <vide/types/std_tuple.hpp>
#include <vide/types/std_unordered_map.hpp>
#include <vide/types/std_unordered_set.hpp>
#include <vide/types/std_pair.hpp>
#include <vide/types/std_vector.hpp>

#include <vide/archives/binary.hpp>
#include <vide/archives/portable_binary.hpp>
#include <vide/archives/xml.hpp>
#include <vide/archives/json.hpp>

#include <iostream>
#include <type_traits>
#include <functional>

//VIDE_FORCE_LINK_SHARED_LIBRARY(Sandbox)

struct Archive {
	static constexpr bool is_output = true;
};

struct Test {
	template <class Archive>
	void serialize(Archive&) {
		std::cout << "hey there" << std::endl;
	}

	template <class Archive>
	void save(Archive&) const {
		std::cout << "saved by the bell" << std::endl;
	}

	template <class Archive>
	void load(Archive&) {
		std::cout << "locked and loaded" << std::endl;
	}

	template <class Archive>
	int save_minimal(const Archive&) const {
		return 0;
	}

	template <class Archive>
	int save_minimal(const Archive&, const std::uint32_t) const {
		return 1;
	}

//	template <class Archive>
//	void load_minimal(const Archive&, int) {}
	template <class Archive>
	void load_minimal(const Archive&, const int&) {}
};

struct A {
	virtual void foo() = 0;

	virtual ~A() {}
};

struct B : A {
	virtual ~B() {}

	void foo() override {}

	template <class Archive>
	void serialize(Archive&) {
		std::cout << "i'm in your b" << std::endl;
	}
};

struct C {
	char a;
};

VIDE_REGISTER_TYPE(B)
VIDE_REGISTER_POLYMORPHIC_RELATION(A, B)

class MemberMinimal {
public:
	MemberMinimal() = default;

	template <class Archive>
	int save_minimal(const Archive&) const {
		return x;
	}

	template <class Archive>
	void load_minimal(const Archive&, int const& str) {
		x = str;
	}

public:
	int x;
};

int main() {
	typedef Test T;
	std::cout << std::boolalpha;

	std::cout << "\tserialize" << std::endl;
	std::cout << vide::access::has_member_serialize<Archive, T> << std::endl;
	std::cout << vide::access::has_member_serialize_versioned<Archive, T> << std::endl;

	std::cout << "\tserializable" << std::endl;
	std::cout << vide::serializable_with<Archive, T> << std::endl;

	std::cout << "\tmember shapes" << std::endl;
	std::cout << vide::access::count_member_shapes<Archive, T> << std::endl;

	std::cout << "\tversioned / minimal" << std::endl;
	std::cout << vide::is_versioned_class<T> << std::endl;
	std::cout << vide::access::has_member_minimal<Archive, MemberMinimal> << std::endl;

	std::cout << "\tarray size" << std::endl;
	std::cout << typeid(A).name() << std::endl;

	// DLL testing
	std::cout << "------DLL TESTING------" << std::endl;
	std::stringstream dllSS1;
	std::stringstream dllSS2;
	{
		vide::XMLOutputArchive out(dllSS1);
		VersionTest x{1};
		std::shared_ptr<Base> p = std::make_shared<Derived>();
		out(x);
		out(p);

		std::shared_ptr<A> ay = std::make_shared<B>();
		out(ay);
	}

	std::cout << dllSS1.str() << std::endl;

	{
		VersionTest x;
		std::shared_ptr<Base> p;
		std::shared_ptr<A> ay;
		{
			vide::XMLInputArchive in(dllSS1);
			in(x);
			in(p);
			in(ay);
		}
		{
			vide::XMLOutputArchive out(dllSS2);
			out(x);
			out(p);
			out(ay);
		}
	}

	std::cout << dllSS2.str() << std::endl;

	return 0;
}
