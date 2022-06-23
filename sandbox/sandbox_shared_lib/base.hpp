#pragma once

#include <memory>
#include <vide/vide.hpp>
#include <vide/archives/xml.hpp>
#include <vide/types/polymorphic.hpp>

#if defined (_WINDLL)
#define DECLSPECIFIER __declspec(dllexport)
#elif defined(MSC_VER)
#define DECLSPECIFIER __declspec(dllimport)
#else
#define DECLSPECIFIER
#endif

int doit();

class VersionTest
{
  public:
    int x;
    template <class Archive>
    void serialize( Archive & ar, const std::uint32_t /* version */ )
    { ar( x ); }
};

class Base
{
  public:
    friend class vide::access;

    template < class Archive >
    void serialize(Archive &, const std::uint32_t) {}
    virtual ~Base() {}
};

extern template DECLSPECIFIER void Base::serialize<vide::XMLInputArchive>
( vide::XMLInputArchive & ar, const std::uint32_t version );

extern template DECLSPECIFIER void Base::serialize<vide::XMLOutputArchive>
( vide::XMLOutputArchive & ar, const std::uint32_t version );

VIDE_CLASS_VERSION(VersionTest, 1)
