// Created by Vader on 2022.02.17..

#pragma once

// std
#include <type_traits>


namespace vide {

// -------------------------------------------------------------------------------------------------

template <typename Ar>
struct underlying_archive;

template <typename Ar>
	requires Ar::is_proxy
struct underlying_archive<Ar> {
	using type = typename Ar::underlying_archive;
};

template <typename Ar>
	requires (!Ar::is_proxy)
struct underlying_archive<Ar> {
	using type = Ar;
};

template <typename Ar>
using underlying_archive_t = typename underlying_archive<Ar>::type;

template <typename Ar>
[[nodiscard]] constexpr inline auto& to_underlying_ar(Ar& ar) noexcept {
	if constexpr (Ar::is_proxy)
		return ar.underlying();
	else
		return ar;
}

// -------------------------------------------------------------------------------------------------

} // namespace vide
