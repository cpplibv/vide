/*! \file access.hpp
    \brief Access control and default construction */
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

#include <utility>

#include <cereal/access_fwd.hpp>
#include <cereal/construct_fwd.hpp>
#include <cereal/macros.hpp>


namespace cereal {

// -------------------------------------------------------------------------------------------------

//! A class that can be made a friend to give cereal access to non public functions
/*! If you desire non-public serialization functions within a class, cereal can only
	access these if you declare cereal::access a friend.

	@code{.cpp}
	class MyClass
	{
	  private:
		friend class cereal::access; // gives access to the private serialize

		template <class Archive>
		void serialize( Archive & ar )
		{
		  // some code
		}
	};
	@endcode
	@ingroup Access */
class access {
public:
	// ####### Standard Serialization ########################################
	template <class Archive, class T> inline
	static auto member_serialize(Archive& ar, T& t) -> decltype(t.CEREAL_SERIALIZE_FUNCTION_NAME(ar)) { return t.CEREAL_SERIALIZE_FUNCTION_NAME(ar); }

	template <class Archive, class T> inline
	static auto member_save(Archive& ar, T const& t) -> decltype(t.CEREAL_SAVE_FUNCTION_NAME(ar)) { return t.CEREAL_SAVE_FUNCTION_NAME(ar); }

	template <class Archive, class T> inline
	static auto member_save_non_const(Archive& ar, T& t) -> decltype(t.CEREAL_SAVE_FUNCTION_NAME(ar)) { return t.CEREAL_SAVE_FUNCTION_NAME(ar); }

	template <class Archive, class T> inline
	static auto member_load(Archive& ar, T& t) -> decltype(t.CEREAL_LOAD_FUNCTION_NAME(ar)) { return t.CEREAL_LOAD_FUNCTION_NAME(ar); }

	template <class Archive, class T> inline
	static auto member_save_minimal(Archive const& ar, T const& t) -> decltype(t.CEREAL_SAVE_MINIMAL_FUNCTION_NAME(ar)) { return t.CEREAL_SAVE_MINIMAL_FUNCTION_NAME(ar); }

	template <class Archive, class T> inline
	static auto member_save_minimal_non_const(Archive const& ar, T& t) -> decltype(t.CEREAL_SAVE_MINIMAL_FUNCTION_NAME(ar)) { return t.CEREAL_SAVE_MINIMAL_FUNCTION_NAME(ar); }

	template <class Archive, class T, class U> inline
	static auto member_load_minimal(Archive const& ar, T& t, U&& u) -> decltype(t.CEREAL_LOAD_MINIMAL_FUNCTION_NAME(ar, std::forward<U>(u))) { return t.CEREAL_LOAD_MINIMAL_FUNCTION_NAME(ar, std::forward<U>(u)); }

	// ####### Versioned Serialization #######################################
	template <class Archive, class T> inline
	static auto member_serialize(Archive& ar, T& t, const std::uint32_t version) -> decltype(t.CEREAL_SERIALIZE_FUNCTION_NAME(ar, version)) { return t.CEREAL_SERIALIZE_FUNCTION_NAME(ar, version); }

	template <class Archive, class T> inline
	static auto member_save(Archive& ar, T const& t, const std::uint32_t version) -> decltype(t.CEREAL_SAVE_FUNCTION_NAME(ar, version)) { return t.CEREAL_SAVE_FUNCTION_NAME(ar, version); }

	template <class Archive, class T> inline
	static auto member_save_non_const(Archive& ar, T& t, const std::uint32_t version) -> decltype(t.CEREAL_SAVE_FUNCTION_NAME(ar, version)) { return t.CEREAL_SAVE_FUNCTION_NAME(ar, version); }

	template <class Archive, class T> inline
	static auto member_load(Archive& ar, T& t, const std::uint32_t version) -> decltype(t.CEREAL_LOAD_FUNCTION_NAME(ar, version)) { return t.CEREAL_LOAD_FUNCTION_NAME(ar, version); }

	template <class Archive, class T> inline
	static auto member_save_minimal(Archive const& ar, T const& t, const std::uint32_t version) -> decltype(t.CEREAL_SAVE_MINIMAL_FUNCTION_NAME(ar, version)) { return t.CEREAL_SAVE_MINIMAL_FUNCTION_NAME(ar, version); }

	template <class Archive, class T> inline
	static auto member_save_minimal_non_const(Archive const& ar, T& t, const std::uint32_t version) -> decltype(t.CEREAL_SAVE_MINIMAL_FUNCTION_NAME(ar, version)) { return t.CEREAL_SAVE_MINIMAL_FUNCTION_NAME(ar, version); }

	template <class Archive, class T, class U> inline
	static auto member_load_minimal(Archive const& ar, T& t, U&& u, const std::uint32_t version) -> decltype(t.CEREAL_LOAD_MINIMAL_FUNCTION_NAME(ar, std::forward<U>(u), version)) { return t.CEREAL_LOAD_MINIMAL_FUNCTION_NAME(ar, std::forward<U>(u), version); }

	// ####### Other Functionality ##########################################
	// for detecting inheritance from enable_shared_from_this
	template <class T> inline
	static auto shared_from_this(T& t) -> decltype(t.shared_from_this());

	// for placement new
	template <class T, class ... Args> inline
	static void construct(T*& ptr, Args&& ... args) {
		new(ptr) T(std::forward<Args>(args)...);
	}

	// for non-placement new with a default constructor
	template <class T> inline
	static T* construct() {
		return new T();
	}
};

// -------------------------------------------------------------------------------------------------

} // namespace cereal
