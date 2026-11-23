//

#pragma once

#include <type_traits>

#include <vide/details/util.hpp>

#include <doctest/doctest.h>


/// Calls fn.template operator()<T>() for each T in order; failures inside name the type:
/// for_each_type<int, float>([]<typename T> { ... })
template <typename... Ts>
void for_each_type(auto&& fn) {
	([&] {
		INFO("type: ", vide::util::demangledName<Ts>());
		fn.template operator()<Ts>();
	}(), ...);
}
