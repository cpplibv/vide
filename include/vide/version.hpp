/*! \file version.hpp
    \brief Macros to detect Vide version

    These macros can assist in determining the version of Vide. Be
    warned that vide is not guaranteed to be compatible across
    different versions. For more information on releases of vide,
    see https://github.com/USCiLab/cereal/releases.

    \ingroup utility */
/*
  Copyright (c) 2018, Shane Grant
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

#ifndef VIDE_VERSION_HPP_
#define VIDE_VERSION_HPP_

//! The major version
#define VIDE_VERSION_MAJOR 2
//! The minor version
#define VIDE_VERSION_MINOR 2
//! The patch version
#define VIDE_VERSION_PATCH 0

//! The full version as a single number
#define VIDE_VERSION (VIDE_VERSION_MAJOR * 10000 \
                        + VIDE_VERSION_MINOR * 100 \
                        + VIDE_VERSION_PATCH)

#endif // VIDE_VERSION_HPP_
