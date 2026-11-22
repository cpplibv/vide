#pragma once

#include <vide/serializer.hpp>

#include <cstdint>
#include <functional>


namespace vide { // --------------------------------------------------------------------------------

template <class T>
struct serializer<std::less<T>> {
	template <class Archive>
	static void serialize(Archive&, std::less<T>&, std::uint32_t) {}
};

template <class T>
struct serializer<std::less_equal<T>> {
	template <class Archive>
	static void serialize(Archive&, std::less_equal<T>&, std::uint32_t) {}
};

template <class T>
struct serializer<std::greater<T>> {
	template <class Archive>
	static void serialize(Archive&, std::greater<T>&, std::uint32_t) {}
};

template <class T>
struct serializer<std::greater_equal<T>> {
	template <class Archive>
	static void serialize(Archive&, std::greater_equal<T>&, std::uint32_t) {}
};

} // namespace vide --------------------------------------------------------------------------------
