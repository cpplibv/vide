#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

#include <vide/binary_data.hpp>
#include <vide/details/diagnostic.hpp>
#include <vide/serializer.hpp>


namespace vide { // --------------------------------------------------------------------------------

namespace detail {

template <class T>
consteval std::string_view raw_pointer_message() {
	return diagnostic_message(
			"    ERROR: Vide cannot serialize the raw pointer [" + diagnostic_name<T>() + "].\n"
			"    Raw pointers carry no ownership: use a smart pointer (std::unique_ptr, std::shared_ptr) instead.\n");
}

} // namespace detail

/// Rejects raw pointers with a compile error, a missing serializer would hide the reason
template <class T>
struct serializer<T*> {
	template <class Archive>
	static void serialize(Archive&, T*&, std::uint32_t) {
		static_assert(false, detail::raw_pointer_message<T*>());
	}
};

/// Serialization for C style arrays
template <class T, std::size_t N>
struct serializer<T[N]> {
	template <class Archive>
	static void serialize(Archive& ar, T (&array)[N], std::uint32_t) {
		if constexpr (Archive::template supports_binary<std::remove_all_extents_t<T>>)
			ar(binary_data(array, sizeof(array)));
		else
			for (auto& i : array)
				ar(i);
	}
};

} // namespace vide --------------------------------------------------------------------------------
