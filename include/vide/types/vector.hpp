/*! \file vector.hpp
    \brief Support for types found in \<vector\>
    \ingroup STLSupport */
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

#include <vector>

#include <vide/binary_data.hpp>
#include <vide/macros.hpp>
#include <vide/size_tag.hpp>


namespace vide {

// -------------------------------------------------------------------------------------------------

//! Serialization for non-arithmetic vector types
template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::vector<T, A>& vector) {
	// TODO P1: Switch to a more generic concept from is_arithmetic_v to determine if a type is binary serializable
	constexpr bool binary_serializable = std::is_arithmetic_v<T>;
	constexpr bool serialize_as_binary = binary_serializable && Archive::template could_serialize<BinaryData<T>>;

	ar(make_size_tag(static_cast<size_type>(vector.size()))); // number of elements

	if constexpr (serialize_as_binary) {
		ar(binary_data(vector.data(), vector.size() * sizeof(T)));
	} else {
		for (const auto& v : vector)
			ar(v);
	}
}

//! Serialization for non-arithmetic vector types
template <class Archive, class T, class A>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::vector<T, A>& vector) {
	// TODO P1: Switch to a more generic concept from is_arithmetic_v to determine if a type is binary serializable
	constexpr bool binary_serializable = std::is_arithmetic_v<T>;
	constexpr bool serialize_as_binary = binary_serializable && Archive::template could_serialize<BinaryData<T>>;

	size_type size;
	ar(make_size_tag(size));
	vector.resize(static_cast<std::size_t>(size));

	if constexpr (serialize_as_binary) {
		ar(binary_data(vector.data(), vector.size() * sizeof(T)));
	} else {
		for (auto& v : vector)
			ar(v);
	}
}

//! Serialization for bool vector types
template <class Archive, class A> inline
void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::vector<bool, A> const& vector) {
	ar(make_size_tag(static_cast<size_type>(vector.size()))); // number of elements
	for (const auto v : vector)
		ar(static_cast<bool>(v));
}

//! Serialization for bool vector types
template <class Archive, class A> inline
void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::vector<bool, A>& vector) {
	size_type size;
	ar(make_size_tag(size));

	vector.resize(static_cast<std::size_t>( size ));
	for (auto&& v : vector) {
		bool b;
		ar(b);
		v = b;
	}
}

// -------------------------------------------------------------------------------------------------

} // namespace vide
