#pragma once

#include <vide/access.hpp>

#include <concepts>
#include <cstdint>
#include <type_traits>
#include <utility>


namespace vide { // --------------------------------------------------------------------------------

namespace detail {

template <typename T>
struct class_version_of {};

template <typename T>
	requires access::has_static_member_class_version<T>
struct class_version_of<T> {
	static constexpr std::uint32_t serialize_class_version = access::static_member_class_version<T>();
};

} // namespace detail

/// The single serialization entry point of T: archives call nothing else. Specialize it for
/// non-intrusive types with a static serialize(Archive&, T&, std::uint32_t version); optional
/// members: serialize_class_version (stored once per type) and minimal (see minimal_with).
/// The primary template forwards to whichever member shape T has: serialize, save/load,
/// save_minimal/load_minimal or serialize_minimal, each with or without a version parameter
template <typename T>
struct serializer : detail::class_version_of<T> {
	// Member minimal shapes are archive templates, so minimal is only known per archive
	template <typename Archive>
	static constexpr bool minimal_with = access::has_member_minimal<Archive, T>;

	template <typename Archive>
		requires (access::count_member_shapes<Archive, T> != 0)
	static void serialize(Archive& ar, T& var, std::uint32_t version) {
		access::serialize_members(ar, var, version);
	}
};

/// True when serializer<T> declares serialize_class_version
template <typename T>
concept is_versioned_class = requires {
	{ serializer<T>::serialize_class_version } -> std::convertible_to<std::uint32_t>;
};

/// True when T is minimal with Archive: serializer<T> declares minimal = true, or the primary
/// template found a member minimal shape. serialize() then makes exactly one unnamed ar(value)
/// call and text archives open no node for it: "d": 5 instead of "d": {"value0": 5}
template <typename Archive, typename T>
concept minimal_with =
		requires { requires serializer<T>::minimal; } ||
		requires { requires serializer<T>::template minimal_with<Archive>; };

/// True when Archive can serialize T through serializer<T>
template <typename Archive, typename T>
concept serializable_with = requires (Archive& ar, T& var) {
	serializer<T>::serialize(ar, var, std::uint32_t{0});
};

} // namespace vide --------------------------------------------------------------------------------
