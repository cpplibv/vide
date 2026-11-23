//

#pragma once

#include <sstream>
#include <string>
#include <tuple>

#include <vide/exception.hpp>


// Ar is an Archives<Input, Output> pair (common/archives.hpp). Each call uses fresh archives, so values never share
// archive state with an earlier call; pass several values to one roundtrip() to test them in sequence

/// Archive content after saving value with the given validators
template <typename Ar, typename T>
[[nodiscard]] std::string save(const T& value, const auto&... validators) {
	std::ostringstream os;
	{
		typename Ar::output ar(os);
		ar(value, validators...);
	}
	return std::move(os).str();
}

/// Loads archive content into value; for types that cannot be returned (std::atomic)
template <typename Ar, typename T>
void load_into(const std::string& data, T& value, const auto&... validators) {
	std::istringstream is(data);
	typename Ar::input ar(is);
	ar(value, validators...);
}

/// Value loaded from archive content, starting from a value initialized T
template <typename Ar, typename T>
[[nodiscard]] T load(const std::string& data, const auto&... validators) {
	T value{};
	load_into<Ar>(data, value, validators...);
	return value;
}

/// Saves and loads back: roundtrip<Ar>(x) -> T, roundtrip<Ar>(x, y) -> std::tuple<X, Y> through one archive
template <typename Ar, typename T, typename... Ts>
[[nodiscard]] auto roundtrip(const T& value, const Ts&... values) {
	std::ostringstream os;
	{
		typename Ar::output ar(os);
		ar(value);
		(ar(values), ...);
	}

	std::tuple<T, Ts...> loaded{};
	std::istringstream is(std::move(os).str());
	{
		typename Ar::input ar(is);
		std::apply([&ar](auto&... v) { (ar(v), ...); }, loaded);
	}

	if constexpr (sizeof...(Ts) == 0)
		return std::get<0>(std::move(loaded));
	else
		return loaded;
}

/// Saves as From, loads as To; for formats shared by two types (multiset -> set)
template <typename Ar, typename To, typename From>
[[nodiscard]] To roundtrip_as(const From& value) {
	return load<Ar, To>(save<Ar>(value));
}

/// Whether saving value passes the validators; any other exception propagates
template <typename Ar, typename T>
[[nodiscard]] bool saves(const T& value, const auto&... validators) {
	try {
		(void) save<Ar>(value, validators...);
		return true;
	} catch (const vide::Exception&) {
		return false;
	}
}

/// Whether loading value back passes the validators; value is saved unchecked, so invalid data reaches the input
template <typename Ar, typename T>
[[nodiscard]] bool loads(const T& value, const auto&... validators) {
	const std::string data = save<Ar>(value);
	try {
		(void) load<Ar, T>(data, validators...);
		return true;
	} catch (const vide::Exception&) {
		return false;
	}
}
