/*! \file valarray.hpp
\brief Support for types found in \<valarray\>
\ingroup STLSupport */

/*
Copyright (c) 2014, Randolph Voorhies, Shane Grant
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of cereal nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <valarray>

#include <cereal/binary_data.hpp>
#include <cereal/macros.hpp>
#include <cereal/size_tag.hpp>


namespace cereal {

// -------------------------------------------------------------------------------------------------

//! Saving for std::valarray all other types
template <class Archive, class T>
inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const std::valarray<T>& valarray) {
	// TODO P1: Switch to a more generic concept from is_arithmetic_v to determine if a type is binary serializable
	constexpr bool binary_serializable = std::is_arithmetic_v<T>;
	constexpr bool serialize_as_binary = binary_serializable && Archive::template could_serialize<BinaryData<T>>;

	ar(make_size_tag(static_cast<size_type>(valarray.size()))); // number of elements

	if constexpr (serialize_as_binary) {
		ar(binary_data(&valarray[0], valarray.size() * sizeof(T)));

	} else {
		for (const auto& i : valarray)
			ar(i);
	}
}

//! Loading for std::valarray all other types
template <class Archive, class T>
inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, std::valarray<T>& valarray) {
	// TODO P1: Switch to a more generic concept from is_arithmetic_v to determine if a type is binary serializable
	constexpr bool binary_serializable = std::is_arithmetic_v<T>;
	constexpr bool serialize_as_binary = binary_serializable && Archive::template could_serialize<BinaryData<T>>;

	size_type valarraySize;
	ar(make_size_tag(valarraySize));

	valarray.resize(static_cast<size_t>(valarraySize));

	if constexpr (serialize_as_binary) {
		ar(binary_data(&valarray[0], static_cast<std::size_t>(valarraySize) * sizeof(T)));

	} else {
		for (auto&& i : valarray)
			ar(i);
	}
}

// -------------------------------------------------------------------------------------------------

} // namespace cereal
