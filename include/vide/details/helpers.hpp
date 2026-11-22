/*! \file helpers.hpp
    \brief Internal helper functionality
    \ingroup Internal */
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
#ifndef VIDE_DETAILS_HELPERS_HPP_
#define VIDE_DETAILS_HELPERS_HPP_

#include <cstdint>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include <vide/macros.hpp>
// #include <vide/map_item.hpp>


namespace vide {

namespace detail {

struct DeferredDataCore {}; //!< Traits struct for DeferredData

} // namespace detail

// ######################################################################
//! A wrapper around data that should be serialized after all non-deferred data
/*! This class is used to demarcate data that can only be safely serialized after
	any data not wrapped in this class.

	@internal */
template <class T>
class DeferredData : public detail::DeferredDataCore {
private:
	// If we get passed an array, keep the type as is, otherwise store
	// a reference if we were passed an l value reference, else copy the value
	using Type = typename std::conditional<std::is_array<typename std::remove_reference<T>::type>::value,
			typename std::remove_cv<T>::type,
			typename std::conditional<std::is_lvalue_reference<T>::value,
					T,
					typename std::decay<T>::type>::type>::type;

	// prevent nested nvps
	static_assert(!std::is_base_of<detail::DeferredDataCore, T>::value, "Cannot defer DeferredData");

	DeferredData& operator=(DeferredData const&) = delete;

public:
	//! Constructs a new NameValuePair
	/*! @param v The value to defer.  Ideally this should be an l-value reference so that
				 the value can be both loaded and saved to.  If you pass an r-value reference,
				 the DeferredData will store a copy of it instead of a reference.  Thus you should
				 only pass r-values in cases where this makes sense, such as the result of some
				 size() call.
		@internal */
	explicit DeferredData(T&& v) : value(std::forward<T>(v)) {}

	Type value;
};

// ######################################################################

namespace detail {

// forward decls for polymorphic support
template <class Archive, class T> struct polymorphic_serialization_support;
struct adl_tag;


} // namespace detail
} // namespace vide

#endif // VIDE_DETAILS_HELPERS_HPP_
