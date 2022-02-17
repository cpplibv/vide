#pragma once
#include "base.hpp"
class Derived : public Base
{
  public:
    virtual ~Derived() {}

  private:
    friend class cereal::access;
    template <class Archive>
    void serialize(Archive & ar, const std::uint32_t)
    {
      ar(cereal::base_class<Base>(this));
    }
};

extern template DECLSPECIFIER void Derived::serialize<cereal::XMLOutputArchive>
    ( cereal::XMLOutputArchive & ar, const std::uint32_t version );
extern template DECLSPECIFIER void Derived::serialize<cereal::XMLInputArchive>
    ( cereal::XMLInputArchive & ar, const std::uint32_t version );

CEREAL_REGISTER_TYPE(Derived)
