#pragma once

#include <vide/access.hpp>

#include <concepts>
#include <cstdint>


namespace vide { // --------------------------------------------------------------------------------

/// The single serialization entry point of T: archives call nothing else.
///
/// Left unspecialized, T serializes through its own member functions, exactly one shape of:
/// serialize, save/load, save_minimal/load_minimal or serialize_minimal, each with or without a
/// trailing std::uint32_t version. Private members work with friend class vide::access.
///
/// @code{.cpp}
/// struct Player {
/// 	std::string name;
/// 	int score;
///
/// 	template <class Archive> void serialize(Archive& ar) {
/// 		ar.nvp("name", name);
/// 		ar.nvp("score", score);
/// 	}
/// };
///
/// class Inventory {
/// 	friend class vide::access;
///
/// 	std::vector<Item> items;
/// 	std::size_t totalWeight = 0; // Derived, not stored
///
/// 	template <class Archive> void save(Archive& ar) const {
/// 		ar.nvp("items", items);
/// 	}
///
/// 	template <class Archive> void load(Archive& ar) {
/// 		ar.nvp("items", items);
/// 		totalWeight = computeWeight(items);
/// 	}
/// };
/// @endcode
///
/// A static serialize_class_version is stored once per type and archive; the versioned shapes
/// receive the stored one on load. Minimal shapes store one bare value: "id": 5, not
/// "id": {"value0": 5}.
///
/// @code{.cpp}
/// struct Config {
/// 	static constexpr std::uint32_t serialize_class_version = 2;
///
/// 	int timeoutMs;
///
/// 	template <class Archive> void serialize(Archive& ar, std::uint32_t version) {
/// 		if constexpr (Archive::is_input)
/// 			if (version < 2) {
/// 				timeoutMs = ar.template nvp_load<int>("timeoutSec") * 1000;
/// 				return;
/// 			}
/// 		ar.nvp("timeoutMs", timeoutMs);
/// 	}
/// };
///
/// struct Id {
/// 	std::int32_t value;
///
/// 	template <class Archive> std::int32_t& serialize_minimal(Archive&) {
/// 		return value;
/// 	}
/// };
/// @endcode
///
/// Types that cannot be modified get a specialization with a static
/// serialize(Archive&, T&, std::uint32_t version). Optional members: serialize_class_version, and
/// minimal = true, after which serialize must make exactly one unnamed ar(value) call.
///
/// @code{.cpp}
/// template <>
/// struct vide::serializer<Point> {
/// 	static constexpr std::uint32_t serialize_class_version = 1;
///
/// 	template <class Archive>
/// 	static void serialize(Archive& ar, Point& p, std::uint32_t) {
/// 		ar.nvp("x", p.x);
/// 		ar.nvp("y", p.y);
/// 	}
/// };
///
/// template <>
/// struct vide::serializer<Meters> {
/// 	static constexpr bool minimal = true;
///
/// 	template <class Archive>
/// 	static void serialize(Archive& ar, Meters& m, std::uint32_t) {
/// 		ar(m.value);
/// 	}
/// };
/// @endcode
///
/// A specialization ignores the serialize_class_version of T itself unless it derives from
/// copy_class_version_of<T>, as the default template does:
///
/// @code{.cpp}
/// template <>
/// struct vide::serializer<Legacy> : vide::copy_class_version_of<Legacy> {
/// 	template <class Archive>
/// 	static void serialize(Archive& ar, Legacy& l, std::uint32_t version);
/// };
/// @endcode
template <typename T>
struct serializer : copy_class_version_of<T> {
	/// A specialization serializer<T> deriving from serializer<U> inherits `default_for = U` here,
	/// so it is not mistaken for the default
	using default_for = T;
};

// -------------------------------------------------------------------------------------------------

/// True when serializer<T> is the default template: T serializes through its member shape
template <typename T>
concept uses_default_serializer = requires {
	requires std::same_as<typename serializer<T>::default_for, T>;
};

/// True when serializer<T> declares serialize_class_version; the default template copies it from T
template <typename T>
concept is_versioned_class = requires {
	{ serializer<T>::serialize_class_version } -> std::convertible_to<std::uint32_t>;
};

/// True when T is minimal with Archive: serializer<T> declares minimal = true, or T has a member
/// minimal shape for the direction of Archive. serialize() then makes exactly one unnamed ar(value)
/// call and text archives open no node for it: "d": 5 instead of "d": {"value0": 5}
template <typename Archive, typename T>
concept minimal_with =
		(uses_default_serializer<T> && access::has_member_minimal<Archive, T>) ||
		requires {
			requires serializer<T>::minimal;
		};

/// True when Archive can serialize T: through serializer<T>, or through the member shape of T
template <typename Archive, typename T>
concept serializable_with =
		(uses_default_serializer<T> && (access::count_member_shapes<Archive, T> != 0)) ||
		requires (Archive& ar, T& var) {
			serializer<T>::serialize(ar, var, std::uint32_t{0});
		};

} // namespace vide --------------------------------------------------------------------------------
