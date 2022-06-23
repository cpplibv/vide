#pragma once
#include "base.hpp"
class Derived : public Base
{
  public:
    virtual ~Derived() {}

  private:
    friend class vide::access;
    template <class Archive>
    void serialize(Archive & ar, const std::uint32_t)
    {
      ar(vide::base_class<Base>(this));
    }
};

extern template DECLSPECIFIER void Derived::serialize<vide::XMLOutputArchive>
    ( vide::XMLOutputArchive & ar, const std::uint32_t version );
extern template DECLSPECIFIER void Derived::serialize<vide::XMLInputArchive>
    ( vide::XMLInputArchive & ar, const std::uint32_t version );

VIDE_REGISTER_TYPE(Derived)
