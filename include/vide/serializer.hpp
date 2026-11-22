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
		static_assert(access::count_member_shapes<Archive, T> == 1, "\n"
				"Vide found more than one member serialization function for the provided type and archive combination.\n\n"
				"Types must either have a serialize function, load/save pair, load_minimal/save_minimal pair or serialize_minimal\n"
				"(you may not mix these, nor versioned and non-versioned forms). Inherited serialization functions count too.\n"
				"Specialize vide::serializer<T> to pick one explicitly.\n\n");

		if constexpr (access::has_member_serialize<Archive, T>) {
			access::member_serialize(ar, var);
		} else if constexpr (access::has_member_serialize_versioned<Archive, T>) {
			access::member_serialize(ar, var, version);
		} else if constexpr (access::has_member_serialize_minimal<Archive, T>) {
			ar(access::member_serialize_minimal(ar, var));
		} else if constexpr (access::has_member_serialize_minimal_versioned<Archive, T>) {
			ar(access::member_serialize_minimal(ar, var, version));
		} else if constexpr (Archive::is_output) {
			if constexpr (access::has_member_save<Archive, T>)
				access::member_save(ar, var);
			else if constexpr (access::has_member_save_versioned<Archive, T>)
				access::member_save(ar, var, version);
			else if constexpr (access::has_member_save_minimal<Archive, T>)
				ar(access::member_save_minimal(ar, var));
			else
				ar(access::member_save_minimal(ar, var, version));
		} else {
			if constexpr (access::has_member_load<Archive, T>) {
				access::member_load(ar, var);
			} else if constexpr (access::has_member_load_versioned<Archive, T>) {
				access::member_load(ar, var, version);
			} else if constexpr (access::has_member_load_minimal<Archive, T>) {
				access::get_member_save_minimal_t<Archive, T> value;
				ar(value);
				access::member_load_minimal(ar, var, std::move(value));
			} else {
				access::get_member_save_minimal_versioned_t<Archive, T> value;
				ar(value);
				access::member_load_minimal(ar, var, std::move(value), version);
			}
		}
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
