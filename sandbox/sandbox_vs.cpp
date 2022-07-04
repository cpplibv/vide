#include <base.hpp>
#include <derived.hpp>

#include <vide/access.hpp>
#include <vide/details/traits.hpp>
#include <vide/details/helpers.hpp>
#include <vide/types/base_class.hpp>
#include <vide/vide.hpp>

#include <vide/types/array.hpp>
#include <vide/types/bitset.hpp>
#include <vide/types/chrono.hpp>
#include <vide/types/common.hpp>
#include <vide/types/complex.hpp>
#include <vide/types/deque.hpp>
#include <vide/types/forward_list.hpp>
#include <vide/types/list.hpp>
#include <vide/types/map.hpp>
#include <vide/types/memory.hpp>

#include <vide/details/util.hpp>

#include <vide/details/polymorphic_impl.hpp>
#include <vide/types/polymorphic.hpp>

#include <vide/types/queue.hpp>
#include <vide/types/set.hpp>
#include <vide/types/stack.hpp>
#include <vide/types/string.hpp>
#include <vide/types/tuple.hpp>
#include <vide/types/unordered_map.hpp>
#include <vide/types/unordered_set.hpp>
#include <vide/types/utility.hpp>
#include <vide/types/vector.hpp>

#include <vide/archives/binary.hpp>
#include <vide/archives/portable_binary.hpp>
#include <vide/archives/xml.hpp>
#include <vide/archives/json.hpp>

#include <iostream>
#include <type_traits>
#include <functional>

//VIDE_FORCE_LINK_SHARED_LIBRARY(Sandbox)

struct Archive {
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

template <class Archive>
void serialize(Archive&, Test&) {}

template <class Archive>
void load(Archive&, Test&) {}

template <class Archive>
void save(Archive&, const Test&) {}

template <class Archive>
int save_minimal(const Archive&, const Test&) { return 0; }

template <class Archive>
int save_minimal(const Archive&, const Test&, const std::uint32_t) { return 0; }

struct A {
	virtual void foo() = 0;

	virtual ~A() {}
};

struct B : A {
	virtual ~B() {}

	void foo() {}

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

	// serialize
	std::cout << "\tserialize" << std::endl;
	std::cout << vide::traits::has_member_serialize<T, Archive>::value << std::endl;
	std::cout << vide::traits::has_non_member_serialize<T, Archive>::value << std::endl;

	// load
	std::cout << "\tload" << std::endl;
	std::cout << vide::traits::has_member_load<T, Archive>::value << std::endl;
	std::cout << vide::traits::has_non_member_load<T, Archive>::value << std::endl;

	// load minimal
	std::cout << "\tload minimal" << std::endl;
	std::cout << vide::traits::has_member_load<T, Archive>::value << std::endl;

	// save
	std::cout << "\tsave" << std::endl;
	std::cout << vide::traits::has_member_save<T, Archive>::value << std::endl;
	std::cout << vide::traits::has_non_member_save<T, Archive>::value << std::endl;

	// save_minimal
	std::cout << "\tsave_minimal" << std::endl;
	std::cout << vide::traits::has_member_save_minimal<T, Archive>::value << std::endl;
	std::cout << vide::traits::has_non_member_save_minimal<T, Archive>::value << std::endl;

	// save_minimal_versioned
	std::cout << "\tsave_minimal versioned" << std::endl;
	std::cout << vide::traits::has_member_versioned_save_minimal<T, Archive>::value << std::endl;
	std::cout << vide::traits::has_non_member_versioned_save_minimal<T, Archive>::value << std::endl;

	// splittable
	std::cout << "\t splittable" << std::endl;
	std::cout << vide::traits::has_member_split<T, Archive, Archive>::value << std::endl;
	std::cout << vide::traits::has_non_member_split<T, Archive, Archive>::value << std::endl;

	// serialiable
	std::cout << "\toutput serializable" << std::endl;
	std::cout << vide::traits::is_output_serializable<T, Archive>::value << std::endl;

#if !defined(__INTEL_COMPILER)
	//! TODO: This causes icc to crash
	std::cout << vide::traits::is_input_serializable<T, Archive>::value << std::endl;
#endif

//	// specialized
//	std::cout << "\tspecialized" << std::endl;
//	std::cout << vide::traits::detail::is_specialized_member_serialize<T, Archive>::value << std::endl;
//	std::cout << vide::traits::detail::is_specialized_member_load_save<T, Archive>::value << std::endl;
//	std::cout << vide::traits::detail::is_specialized_non_member_serialize<T, Archive>::value << std::endl;
//	std::cout << vide::traits::detail::is_specialized_non_member_load_save<T, Archive>::value << std::endl;
//	std::cout << vide::traits::detail::count_specializations<T, Archive>::value << std::endl;
//	std::cout << vide::traits::is_specialized<T, Archive>::value << std::endl;

	// counts
	std::cout << "\tcounts" << std::endl;
	std::cout << vide::traits::detail::count_output_serializers<T, Archive>::value << std::endl;
	std::cout << vide::traits::detail::count_input_serializers<T, Archive>::value << std::endl;

	// array size
	std::cout << "\tarray size" << std::endl;
	std::cout << typeid(A).name() << std::endl;

	// extra testing
	std::cout << "\textra" << std::endl;
	std::cout << vide::traits::has_member_save_minimal<MemberMinimal, Archive>::value << std::endl;
	std::cout << vide::traits::has_member_load_minimal<MemberMinimal, Archive>::value << std::endl;

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
