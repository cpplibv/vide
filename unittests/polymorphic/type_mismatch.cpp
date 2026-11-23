//

#include <vide/types/memory.hpp>
#include <vide/types/polymorphic.hpp>
#include <vide/types/std_tuple.hpp>

#include <memory>
#include <random>
#include <string>
#include <tuple>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/my_intrusive_ptr_polymorphic.hpp"
#include "common/random.hpp"
#include "common/roundtrip.hpp"
#include "test_classes.hpp"


/// Saves pointer twice; loading the second reference through Mismatched (another pointee or pointer type) throws
template <typename Ar, typename Mismatched, typename Pointer>
void check_second_reference_mismatch(const Pointer& pointer) {
	const std::string data = save<Ar>(std::tuple{pointer, pointer});
	CHECK_THROWS_AS((std::ignore = load<Ar, std::tuple<Pointer, Mismatched>>(data)), vide::Exception);
}

TEST_CASE_TEMPLATE("polymorphic pointer type mismatch", Ar, VIDE_ALL_ARCHIVES) {
	std::mt19937 rng = test_rng();
	Initializer init{rng};

	SUBCASE("derived object, unrelated polymorphic pointee") {
		check_second_reference_mismatch<Ar, std::shared_ptr<RootSimpleB>>(
				std::shared_ptr<RootSimpleA>{std::make_shared<LeafNonFinalA_SimpleA>(init)});
	}

	SUBCASE("base object, unrelated polymorphic pointee") {
		check_second_reference_mismatch<Ar, std::shared_ptr<RootSimpleB>>(std::make_shared<RootSimpleA>(init));
	}

	SUBCASE("derived object, non polymorphic pointee") {
		check_second_reference_mismatch<Ar, std::shared_ptr<int>>(
				std::shared_ptr<RootSimpleA>{std::make_shared<LeafNonFinalA_SimpleA>(init)});
	}

	SUBCASE("base object, non polymorphic pointee") {
		check_second_reference_mismatch<Ar, std::shared_ptr<int>>(std::make_shared<RootSimpleA>(init));
	}

	SUBCASE("non polymorphic object, polymorphic pointee") {
		check_second_reference_mismatch<Ar, std::shared_ptr<RootSimpleA>>(std::make_shared<int>(32));
	}

	SUBCASE("same pointee, other smart pointer type") {
		check_second_reference_mismatch<Ar, my::intrusive_ptr<TestRefCountedPolymorphic>>(
				std::make_shared<TestRefCountedPolymorphic>(32));
	}
}
