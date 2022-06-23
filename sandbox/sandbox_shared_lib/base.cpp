#ifndef VIDE_DLL_USE
#define VIDE_DLL_MAKE
#endif
#include "base.hpp"

template void Base::serialize<vide::XMLOutputArchive>
    ( vide::XMLOutputArchive & ar, const std::uint32_t version );
template void Base::serialize<vide::XMLInputArchive>
    ( vide::XMLInputArchive & ar, const std::uint32_t version );
