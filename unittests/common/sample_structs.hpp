//

#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <ostream>
#include <random>
#include <type_traits>

#include <doctest/doctest.h>
#include <vide/serializer.hpp>

#include "for_each_type.hpp"
#include "random.hpp"
#include "serialization_calls.hpp"


// One struct per serialization shape (member / specialized, serialize / split), all with the same two ints;
// container tests use them as element types. Each records its calls (serialization_calls.hpp)

struct StructBase {
	int x = 0;
	int y = 0;

	constexpr StructBase() = default;
	constexpr StructBase(int x, int y) :
		x(x),
		y(y) {}

	constexpr bool operator==(const StructBase&) const = default;
	constexpr auto operator<=>(const StructBase&) const = default;

	friend std::ostream& operator<<(std::ostream& os, const StructBase& s) {
		return os << "[x: " << s.x << " y: " << s.y << "]";
	}
};

struct StructInternalSerialize : StructBase {
	using StructBase::StructBase;

	template <typename Archive>
	void serialize(Archive& ar) {
		record_call<Archive>(this);
		ar(x)(y);
	}
};

struct StructInternalSplit : StructBase {
	using StructBase::StructBase;

	template <typename Archive>
	void save(Archive& ar) const {
		record_call<Archive>(this);
		ar(x)(y);
	}

	template <typename Archive>
	void load(Archive& ar) {
		record_call<Archive>(this);
		ar(x)(y);
	}
};

struct StructExternalSerialize : StructBase {
	using StructBase::StructBase;
};

template <>
struct vide::serializer<StructExternalSerialize> {
	template <typename Archive>
	static void serialize(Archive& ar, StructExternalSerialize& s, std::uint32_t) {
		record_call<Archive>(&s);
		ar(s.x)(s.y);
	}
};

struct StructExternalSplit : StructBase {
	using StructBase::StructBase;
};

template <>
struct vide::serializer<StructExternalSplit> {
	template <typename Archive>
	static void serialize(Archive& ar, StructExternalSplit& s, std::uint32_t) {
		record_call<Archive>(&s);
		if constexpr (Archive::is_output) {
			ar(s.x)(s.y);
		} else {
			int loaded_x;
			int loaded_y;
			ar(loaded_x)(loaded_y);
			s.x = loaded_x;
			s.y = loaded_y;
		}
	}
};

/// Hash for the unordered container tests; works with any of the structs above
template <typename T>
struct StructHash {
	std::size_t operator()(const T& s) const {
		return std::hash<int>{}(s.x) ^ (std::hash<int>{}(s.y) << 1);
	}
};

/// random_value() support: both members random
template <std::derived_from<StructBase> T>
[[nodiscard]] T make_random(std::type_identity<T>, std::mt19937& gen) {
	return T{random_value<int>(gen), random_value<int>(gen)};
}

/// Calls fn.template operator()<S>() for every sample struct; failures inside name the struct. fn has to save and
/// load S: the call fails unless S's own serialization functions ran in both directions
void for_each_sample_struct(auto&& fn) {
	for_each_type<StructInternalSerialize, StructInternalSplit, StructExternalSerialize, StructExternalSplit>([&]<typename S> {
		const CallLog calls;
		fn.template operator()<S>();
		CHECK_GT(calls.of<S>().saves, 0);
		CHECK_GT(calls.of<S>().loads, 0);
	});
}
