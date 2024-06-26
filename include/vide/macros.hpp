/*! \file macros.hpp
    \brief Preprocessor macros that can customise the vide library

    By default, vide looks for serialization functions with very
    specific names, that is: serialize, load, save, load_minimal,
    or save_minimal.

    This file allows an advanced user to change these names to conform
    to some other style or preference.  This is implemented using
    preprocessor macros.

    As a result of this, in internal vide code you will see macros
    used for these function names.  In user code, you should name
    the functions like you normally would and not use the macros
    to improve readability.
    \ingroup utility */
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

#include <cstdint>


#ifndef VIDE_THREAD_SAFE
//! Whether vide should be compiled for a threaded environment
/*! This macro causes vide to use mutexes to control access to
    global internal state in a thread safe manner.

    Note that even with this enabled you must still ensure that
    archives are accessed by only one thread at a time; it is safe
    to use multiple archives in parallel, but not to access one archive
    from many places simultaneously. */
#define VIDE_THREAD_SAFE 0
#endif // VIDE_THREAD_SAFE

#ifndef VIDE_SIZE_TYPE
//! Determines the data type used for size_type
/*! vide uses size_type to ensure that the serialized size of
    dynamic containers is compatible across different architectures
    (e.g. 32 vs 64 bit), which may use different underlying types for
    std::size_t.

    More information can be found in vide/details/helpers.hpp.

    If you choose to modify this type, ensure that you use a fixed
    size type (e.g. uint32_t). */
#define VIDE_SIZE_TYPE uint32_t
#endif // VIDE_SIZE_TYPE

#ifndef VIDE_RESERVE_MEMORY_BUDGET_MULTIPLIER
/// Multipler of the input data size which deretmines the memory budget that can be used to reserve containers.
/// Typical values are 1 .. 8. It is meant as a reasonable upper estimate on how much C++ object memory
/// can be extracted from the archive.
/// If the budget ever runs out the deserialization will fall back to element wise non-reserved allocation patterns.
#define VIDE_RESERVE_MEMORY_BUDGET_MULTIPLIER 8
#endif // VIDE_RESERVE_MEMORY_BUDGET_MULTIPLIER

// =================================================================================================

#ifndef VIDE_FUNCTION_NAME_SERIALIZE
//! The serialization/deserialization function name to search for.
/*! You can define @c VIDE_FUNCTION_NAME_SERIALIZE to be different assuming
    you do so before this file is included. */
#define VIDE_FUNCTION_NAME_SERIALIZE serialize
#endif // VIDE_FUNCTION_NAME_SERIALIZE

#ifndef VIDE_FUNCTION_NAME_LOAD
//! The deserialization (load) function name to search for.
/*! You can define @c VIDE_FUNCTION_NAME_LOAD to be different assuming you do so
    before this file is included. */
#define VIDE_FUNCTION_NAME_LOAD load
#endif // VIDE_FUNCTION_NAME_LOAD

#ifndef VIDE_FUNCTION_NAME_SAVE
//! The serialization (save) function name to search for.
/*! You can define @c VIDE_FUNCTION_NAME_SAVE to be different assuming you do so
    before this file is included. */
#define VIDE_FUNCTION_NAME_SAVE save
#endif // VIDE_FUNCTION_NAME_SAVE

#ifndef VIDE_FUNCTION_NAME_LOAD_MINIMAL
//! The deserialization (load_minimal) function name to search for.
/*! You can define @c VIDE_FUNCTION_NAME_LOAD_MINIMAL to be different assuming you do so
    before this file is included. */
#define VIDE_FUNCTION_NAME_LOAD_MINIMAL load_minimal
#endif // VIDE_FUNCTION_NAME_LOAD_MINIMAL

#ifndef VIDE_FUNCTION_NAME_SAVE_MINIMAL
//! The serialization (save_minimal) function name to search for.
/*! You can define @c VIDE_FUNCTION_NAME_SAVE_MINIMAL to be different assuming you do so
    before this file is included. */
#define VIDE_FUNCTION_NAME_SAVE_MINIMAL save_minimal
#endif // VIDE_FUNCTION_NAME_SAVE_MINIMAL

// =================================================================================================
