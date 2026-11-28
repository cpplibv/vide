#pragma once

#include <vide/details/polymorphic_impl.hpp> // Polymorphic bases register their casters even when serialized by value

#include <type_traits>


namespace vide { // --------------------------------------------------------------------------------

/// Tag of a non-virtual base class subobject, created by ar.base_class<Base>(this).
/// Serialized as the base itself, abstract bases included
template <class Base>
struct BaseClass {
	/// Non-const even when saving: save() const passes a const this, output archives only read through it
	Base* base_ptr;

	template <class Derived>
	explicit BaseClass(const Derived* derived) :
		base_ptr(const_cast<Base*>(static_cast<const Base*>(derived))) {
		static_assert(std::is_base_of_v<Base, Derived>, "BaseClass<Base> requires a class derived from Base");
		// Lets a pointer to Base load a Derived, even when no Derived is ever serialized by value
		if constexpr (std::is_polymorphic_v<Base>)
			detail::RegisterPolymorphicCaster<Base, Derived>::bind();
	}
};

/// Tag of a virtual base class subobject, created by ar.virtual_base_class<Base>(this).
/// Serialized once per object: a diamond reaches the same base on every path, only the first one writes it
template <class Base>
struct VirtualBaseClass {
	/// Non-const even when saving, see BaseClass::base_ptr
	Base* base_ptr;

	template <class Derived>
	explicit VirtualBaseClass(const Derived* derived) :
		base_ptr(const_cast<Base*>(static_cast<const Base*>(derived))) {
		static_assert(std::is_base_of_v<Base, Derived>, "VirtualBaseClass<Base> requires a class derived from Base");
		if constexpr (std::is_polymorphic_v<Base>)
			detail::RegisterPolymorphicCaster<Base, Derived>::bind();
	}
};

} // namespace vide --------------------------------------------------------------------------------
