#ifndef VIDE_DLL_USE
#define VIDE_DLL_MAKE
#endif
#include "derived.hpp"

template void Derived::serialize<vide::XMLOutputArchive>
    ( vide::XMLOutputArchive & ar, const std::uint32_t version );

template void Derived::serialize<vide::XMLInputArchive>
    ( vide::XMLInputArchive & ar, const std::uint32_t version );
