//
// Expected: cannot track the smart pointer \[std::shared_ptr<int>\]: it takes [0-9]+ bytes, VIDE_SMART_PTR_MAX_SIZE allows 4\.
// Expected: Define VIDE_SMART_PTR_MAX_SIZE to at least [0-9]+, project wide

#define VIDE_SMART_PTR_MAX_SIZE 4

#include <vide/archives/binary.hpp>
#include <vide/types/memory.hpp>

#include <memory>
#include <sstream>


void save_it() {
	std::ostringstream os;
	vide::BinaryOutputArchive ar(os);
	ar(std::make_shared<int>(1));
}
