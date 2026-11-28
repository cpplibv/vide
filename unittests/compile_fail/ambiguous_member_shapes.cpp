//
// Expected: more than one member serializer for \[Both\] with archive \[vide::BinaryOutputArchive\]: serialize\(ar\), save\(ar, version\)\.
// Expected: specialize vide::serializer<T> to pick one explicitly

#include <vide/archives/binary.hpp>

#include <sstream>


struct Both {
	int x = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(x);
	}

	template <class Archive>
	void save(Archive& ar, std::uint32_t) const {
		ar(x);
	}
};

void save_it() {
	std::ostringstream os;
	vide::BinaryOutputArchive ar(os);
	ar(Both{});
}
