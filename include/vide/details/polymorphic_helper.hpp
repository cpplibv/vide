//

#pragma once

#include <vide/details/function_ref.hpp>

#include <typeindex>


namespace vide::polymorphic_detail { // ------------------------------------------------------------

struct InputSerializers {

	using UpcastFn = void*(*)(void* objectAddress, const std::type_info& loadingType);
	using GenericSerializer = void(*)(void* arPtr, const std::type_info& loadingType, function_ref<void(void*, void*, const std::type_index&)> registerFn);

	UpcastFn upcast;
	GenericSerializer generic_ptr;
};

} // namespace vide::polymorphic_detail ------------------------------------------------------------
