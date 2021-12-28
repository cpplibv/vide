#pragma once

#include <cstdint>


namespace cereal {
namespace detail {

// -------------------------------------------------------------------------------------------------

// used during saving pointers
static const uint32_t msb_32bit = 0x80000000;
static const int32_t msb2_32bit = 0x40000000;

// -------------------------------------------------------------------------------------------------

} // namespace detail
} // namespace cereal
