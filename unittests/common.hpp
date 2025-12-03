/*
  Copyright (c) 2013-2022, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <vide/types/memory.hpp>
#include <vide/types/array.hpp>
#include <vide/types/atomic.hpp>
#include <vide/types/valarray.hpp>
#include <vide/types/vector.hpp>
#include <vide/types/deque.hpp>
#include <vide/types/forward_list.hpp>
#include <vide/types/list.hpp>
#include <vide/types/string.hpp>
#include <vide/types/map.hpp>
#include <vide/types/queue.hpp>
#include <vide/types/set.hpp>
#include <vide/types/stack.hpp>
#include <vide/types/unordered_map.hpp>
#include <vide/types/unordered_set.hpp>
#include <vide/types/utility.hpp>
#include <vide/types/tuple.hpp>
#include <vide/types/bitset.hpp>
#include <vide/types/complex.hpp>
#include <vide/types/chrono.hpp>
#include <vide/types/polymorphic.hpp>

#include <vide/archives/binary.hpp>
#include <vide/archives/portable_binary.hpp>
#include <vide/archives/xml.hpp>
#include <vide/archives/json.hpp>

#include <vide/archives/proxy_archive.hpp>

#include <limits>
#include <random>

#include "doctest.h"


namespace std {

// Ostream overload for std::pair
template <class F, class S>
inline ::std::ostream& operator<<(::std::ostream& os, ::std::pair<F, S> const& p) {
	os << "([" << p.first << "], [" << p.second << "])";
	return os;
}
}

// Checks that collections have equal size and all elements are the same
template <class T>
inline void check_collection(T const& a, T const& b) {
	auto aIter = std::begin(a);
	auto aEnd = std::end(a);
	auto bIter = std::begin(b);
	auto bEnd = std::end(b);

	CHECK_EQ(std::distance(aIter, aEnd), std::distance(bIter, bEnd));

	for (; aIter != aEnd; ++aIter, ++bIter)
		CHECK_EQ(*aIter, *bIter);
}

template <class T>
inline void check_ptr_collection(T const& a, T const& b) {
	auto aIter = std::begin(a);
	auto aEnd = std::end(a);
	auto bIter = std::begin(b);
	auto bEnd = std::end(b);

	CHECK_EQ(std::distance(aIter, aEnd), std::distance(bIter, bEnd));

	for (; aIter != aEnd; ++aIter, ++bIter)
		CHECK_EQ(**aIter, **bIter);
}

// Random Number Generation ===============================================

template <class T> inline
typename std::enable_if<std::is_floating_point<T>::value, T>::type
random_value(std::mt19937& gen) { return std::uniform_real_distribution<T>(-10000.0, 10000.0)(gen); }

template <class T> inline
typename std::enable_if<std::is_integral<T>::value && sizeof(T) != sizeof(char), T>::type
random_value(std::mt19937& gen) { return std::uniform_int_distribution<T>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen); }

template <class T> inline
typename std::enable_if<std::is_integral<T>::value && sizeof(T) == sizeof(char), T>::type
random_value(std::mt19937& gen) { return static_cast<T>( std::uniform_int_distribution<int64_t>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen)); }

template <class T> inline
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
random_value(std::mt19937& gen) {
	std::string s(std::uniform_int_distribution<int>(3, 30)(gen), ' ');
	for (char& c : s)
		c = static_cast<char>( std::uniform_int_distribution<int>('A', 'Z')(gen));
	return s;
}

size_t random_index(size_t min, size_t max, std::mt19937& gen) {
	return std::uniform_int_distribution<size_t>(min, max)(gen);
}

template <class C> inline
std::basic_string<C> random_basic_string(std::mt19937& gen) {
	std::basic_string<C> s(std::uniform_int_distribution<int>(3, 30)(gen), ' ');
	for (C& c : s)
		c = static_cast<C>( std::uniform_int_distribution<int>('A', 'Z')(gen));
	return s;
}

template <size_t N> inline
std::string random_binary_string(std::mt19937& gen) {
	std::string s(N, ' ');
	for (auto& c : s)
		c = static_cast<char>( std::uniform_int_distribution<int>('0', '1')(gen));
	return s;
}

// Generic struct useful for testing many serialization functions
struct StructBase {
	StructBase() {}
	StructBase(int xx, int yy) : x(xx), y(yy) {}

	int x, y;

	bool operator==(StructBase const& other) const { return x == other.x && y == other.y; }

	bool operator!=(StructBase const& other) const { return x != other.x || y != other.y; }

	bool operator<(StructBase const& other) const {
		if (x < other.x) return true;
		else if (other.x < x) return false;
		else return (y < other.y);
	}
};

inline std::ostream& operator<<(std::ostream& os, StructBase const& s) {
	os << "[x: " << s.x << " y: " << s.y << "]";
	return os;
}

struct StructInternalSerialize : StructBase {
	StructInternalSerialize() : StructBase{0, 0} {}
	StructInternalSerialize(int x_, int y_) : StructBase{x_, y_} {}

	template <class Archive>
	void serialize(Archive& ar) {
		ar(x)(y);
	}
};

struct StructInternalSplit : StructBase {
	StructInternalSplit() : StructBase{0, 0} {}
	StructInternalSplit(int x_, int y_) : StructBase{x_, y_} {}

	template <class Archive>
	void save(Archive& ar) const {
		ar(x)(y);
	}

	template <class Archive>
	void load(Archive& ar) {
		ar(x)(y);
	}
};

struct StructExternalSerialize : StructBase {
	StructExternalSerialize() : StructBase{0, 0} {}
	StructExternalSerialize(int x_, int y_) : StructBase{x_, y_} {}
};

template <class Archive>
void serialize(Archive& ar, StructExternalSerialize& s) {
	ar(s.x)(s.y);
}

struct StructExternalSplit : StructBase {
	StructExternalSplit() : StructBase{0, 0} {}
	StructExternalSplit(int x_, int y_) : StructBase{x_, y_} {}
};

template <class Archive> inline
void save(Archive& ar, StructExternalSplit const& s) {
	ar(s.x)(s.y);
}

template <class Archive> inline
void load(Archive& ar, StructExternalSplit& s) {
	ar(s.x)(s.y);
}

template <class T>
struct StructHash {
public:
	size_t operator()(const T& s) const {
		size_t h1 = std::hash<int>()(s.x);
		size_t h2 = std::hash<int>()(s.y);
		return h1 ^ (h2 << 1);
	}
};

// -------------------------------------------------------------------------------------------------

template <typename Ar>
struct UserProxyArchive : vide::ProxyArchive<UserProxyArchive<Ar>, Ar> {
	using vide::ProxyArchive<UserProxyArchive<Ar>, Ar>::ProxyArchive;

	int my_user_data = 42;
};

template <typename Ar>
struct ProxyStorage {
	Ar ar;

	template <typename T>
	explicit ProxyStorage(T&& ss) : ar(ss) {}
};

template <typename Ar>
struct ProxyTestGroup : ProxyStorage<Ar>, UserProxyArchive<Ar> {
	template <typename T>
	explicit ProxyTestGroup(T&& ss) :
			ProxyStorage<Ar>(ss),
			UserProxyArchive<Ar>(ProxyStorage<Ar>::ar) {}
};

// -------------------------------------------------------------------------------------------------

#define CREATE_TEST_CASES_FOR_BINARY_ARCHIVE(Name, Function)                                                            \
    TEST_CASE("binary_" Name) {                                                                                         \
        Function<vide::BinaryInputArchive, vide::BinaryOutputArchive>();                                                \
    }                                                                                                                   \
                                                                                                                        \
    TEST_CASE("portable_binary_" Name) {                                                                                \
        Function<vide::PortableBinaryInputArchive, vide::PortableBinaryOutputArchive>();                                \
    }

#define CREATE_TEST_CASES_FOR_TEXT_ARCHIVE(Name, Function)                                                              \
    TEST_CASE("xml_" Name) {                                                                                            \
        Function<vide::XMLInputArchive, vide::XMLOutputArchive>();                                                      \
    }                                                                                                                   \
                                                                                                                        \
    TEST_CASE("json_" Name) {                                                                                           \
        Function<vide::JSONInputArchive, vide::JSONOutputArchive>();                                                    \
    }

#define CREATE_TEST_CASES_FOR_PROXY_ARCHIVE_OVER_BINARY(Name, Function)                                                 \
    TEST_CASE("proxy binary_" Name) {                                                                                   \
        Function<ProxyTestGroup<vide::BinaryInputArchive>, ProxyTestGroup<vide::BinaryOutputArchive>>();                \
    }                                                                                                                   \
                                                                                                                        \
    TEST_CASE("proxy portable_binary_" Name) {                                                                          \
        Function<ProxyTestGroup<vide::PortableBinaryInputArchive>, ProxyTestGroup<vide::PortableBinaryOutputArchive>>();\
    }                                                                                                                   \

#define CREATE_TEST_CASES_FOR_PROXY_ARCHIVE_OVER_TEXT(Name, Function)                                                   \
    TEST_CASE("proxy xml_" Name) {                                                                                      \
        Function<ProxyTestGroup<vide::XMLInputArchive>, ProxyTestGroup<vide::XMLOutputArchive>>();                      \
    }                                                                                                                   \
                                                                                                                        \
    TEST_CASE("proxy json_" Name) {                                                                                     \
        Function<ProxyTestGroup<vide::JSONInputArchive>, ProxyTestGroup<vide::JSONOutputArchive>>();                    \
    }

#define CREATE_TEST_CASES_FOR_ALL_ARCHIVE(Name, Function) \
    CREATE_TEST_CASES_FOR_BINARY_ARCHIVE(Name, Function)  \
    CREATE_TEST_CASES_FOR_TEXT_ARCHIVE(Name, Function)  \
    CREATE_TEST_CASES_FOR_PROXY_ARCHIVE_OVER_BINARY(Name, Function) \
    CREATE_TEST_CASES_FOR_PROXY_ARCHIVE_OVER_TEXT(Name, Function)

// -------------------------------------------------------------------------------------------------
