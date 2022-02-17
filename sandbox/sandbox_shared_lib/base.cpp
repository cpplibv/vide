#ifndef CEREAL_DLL_USE
#define CEREAL_DLL_MAKE
#endif
#include "base.hpp"

template void Base::serialize<cereal::XMLOutputArchive>
    ( cereal::XMLOutputArchive & ar, const std::uint32_t version );
template void Base::serialize<cereal::XMLInputArchive>
    ( cereal::XMLInputArchive & ar, const std::uint32_t version );
