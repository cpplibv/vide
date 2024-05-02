/*! \file pair_associative_container.hpp
    \brief Support for the PairAssociativeContainer refinement of the
    AssociativeContainer concept.
    \ingroup TypeConcepts */
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

#include <vide/macros.hpp>
#include <vide/map_item.hpp>
#include <vide/nvp.hpp>
#include <vide/size_tag.hpp>


namespace vide {
//! Saving for std-like pair associative containers
template<class Archive, template <typename...> class Map, typename... Args, typename = typename Map <Args...>::mapped_type>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, Map <Args...> const& map) {
	ar(make_size_tag(static_cast <size_type>(map.size())));

	for (const auto& i : map)
		ar(make_map_item(i.first, i.second));
}

//! Loading for std-like pair associative containers
template<class Archive, template <typename...> class Map, typename... Args, typename = typename Map <Args...>::mapped_type>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, Map <Args...>& map) {
	size_type size;
	ar(make_size_tag(size));

	map.clear();

	auto hint = map.begin();
	for (size_t i = 0; i < size; ++i) {
		typename Map <Args...>::key_type key;
		typename Map <Args...>::mapped_type value;

		ar(make_map_item(key, value));
		hint = map.emplace_hint(hint, std::move(key), std::move(value));
	}
}
} // namespace vide
