//

#pragma once

#include <vide/details/function_ref.hpp>

#include <typeindex>


namespace vide::polymorphic_detail { // ------------------------------------------------------------

struct InputSerializers {

	using UpcastFn = void*(*)(void*, const std::type_info&);
	using GenericSerializer = void(*)(void*, const std::type_info&, function_ref<void(void*, void*, const std::type_index&)>);

	UpcastFn upcast;
	GenericSerializer generic_ptr;
};

} // namespace vide::polymorphic_detail ------------------------------------------------------------
