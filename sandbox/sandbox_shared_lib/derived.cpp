#ifndef CEREAL_DLL_USE
#define CEREAL_DLL_MAKE
#endif
#include "derived.hpp"

template void Derived::serialize<cereal::XMLOutputArchive>
    ( cereal::XMLOutputArchive & ar, const std::uint32_t version );

template void Derived::serialize<cereal::XMLInputArchive>
    ( cereal::XMLInputArchive & ar, const std::uint32_t version );
