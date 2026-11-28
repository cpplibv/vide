//

#pragma once


namespace vide::detail { // ------------------------------------------------------------------------

/// Forward declaration, see polymorphic_impl.hpp for more information
template <class Base, class Derived>
struct RegisterPolymorphicCaster;

/// Forward declaration, see polymorphic_impl.hpp for more information
struct PolymorphicCasters;

/// Forward declaration, see polymorphic_impl.hpp for more information
template <class Base, class Derived>
struct PolymorphicRelation;

/// Forward declaration for VIDE_REGISTER_ARCHIVE, see polymorphic_impl.hpp for more information
template <class Archive, class T>
struct polymorphic_serialization_support;

/// Forward declaration for VIDE_REGISTER_ARCHIVE, see polymorphic_impl.hpp for more information
struct adl_tag;

} // namespace vide::detail ------------------------------------------------------------------------
