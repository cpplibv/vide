//

#pragma once

#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <ostream>

#include <vide/serializer.hpp>


// One struct per serialization shape (member / specialized, serialize / split), all with the same two ints;
// container tests use them as element types

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
		ar(x)(y);
	}
};

struct StructInternalSplit : StructBase {
	using StructBase::StructBase;

	template <typename Archive>
	void save(Archive& ar) const {
		ar(x)(y);
	}

	template <typename Archive>
	void load(Archive& ar) {
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
