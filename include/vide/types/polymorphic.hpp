//

#pragma once

#include <vide/details/helpers.hpp>
#include <vide/details/polymorphic_helper.hpp>
#include <vide/details/polymorphic_impl.hpp>
#include <vide/details/traits.hpp>
#include <vide/details/util.hpp>
#include <vide/details/vide_types.hpp>
#include <vide/exception.hpp>
#include <vide/macros.hpp>
#include <vide/types/memory.hpp>
#include <vide/types/std_string.hpp>

#include <cassert>
#include <string>


//! Registers a derived polymorphic type with vide
/*! Polymorphic types must be registered before smart
    pointers to them can be serialized.  Note that base
    classes do not need to be registered.

    Registering a type lets vide know how to properly
    serialize it when a smart pointer to a base object is
    used in conjunction with a derived class.

    This assumes that all relevant archives have also
    previously been registered.  Registration for archives
    is usually done in the header file in which they are
    defined.  This means that type registration needs to
    happen after specific archives to be used are included.

    It is recommended that type registration be done in
    the header file in which the type is declared.

    Registration can also be placed in a source file,
    but this may require the use of the
    VIDE_REGISTER_DYNAMIC_INIT macro (see below).

    Registration may be called repeatedly for the same
    type in different translation units to add support
    for additional archives if they are not initially
    available (included and registered).

    When building serialization support as a DLL on
    Windows, registration must happen in the header file.
    On Linux and Mac things should still work properly
    if placed in a source file, but see the above comments
    on registering in source files.

    Polymorphic support in vide requires RTTI to be
    enabled */
#define VIDE_REGISTER_TYPE(...)                                       \
	namespace vide::detail {                                          \
	template <>                                                       \
	struct binding_name<__VA_ARGS__> {                                \
		static constexpr char const * name() { return #__VA_ARGS__; } \
	};                                                                \
	} /* namespace vide::detail */                                    \
	VIDE_BIND_TO_ARCHIVES(__VA_ARGS__)

//! Registers a polymorphic type with vide, giving it a
//! user defined name
/*! In some cases the default name used with
    VIDE_REGISTER_TYPE (the name of the type) may not be
    suitable.  This macro allows any name to be associated
    with the type.  The name must be unique */
#define VIDE_REGISTER_TYPE_WITH_NAME(T, Name)                 \
	namespace vide::detail {                                  \
	template <>                                               \
	struct binding_name<T> {                                  \
		static constexpr char const * name() { return Name; } \
	};                                                        \
	} /* namespace vide::detail */                            \
	VIDE_BIND_TO_ARCHIVES(T)

//! Registers the base-derived relationship for a polymorphic type
/*! When polymorphic serialization occurs, vide needs to know how to
    properly cast between derived and base types for the polymorphic
    type. Normally this happens automatically whenever vide::base_class
    or vide::virtual_base_class are used to serialize a base class. In
    cases where neither of these is ever called but a base class still
    exists, this explicit registration is required.

    The Derived class should be the most derived type that will be serialized,
    and the Base type any possible base that has not been covered under a base
    class serialization that will be used to store a Derived pointer.

    Placement of this is the same as for VIDE_REGISTER_TYPE. */
#define VIDE_REGISTER_POLYMORPHIC_RELATION(Base, Derived)     \
	namespace vide::detail {                                  \
	template <>                                               \
	struct PolymorphicRelation<Base, Derived> {               \
		static void bind() {                                  \
			RegisterPolymorphicCaster<Base, Derived>::bind(); \
		}                                                     \
	};                                                        \
	} /* namespace vide::detail */

//! Adds a way to force initialization of a translation unit containing
//! calls to VIDE_REGISTER_TYPE
/*! In C++, dynamic initialization of non-local variables of a translation
    unit may be deferred until "the first odr-use of any function or variable
    defined in the same translation unit as the variable to be initialized."

    Informally, odr-use means that your program takes the address of or binds
    a reference directly to an object, which must have a definition.

    Since polymorphic type support in vide relies on the dynamic
    initialization of certain global objects happening before
    serialization is performed, it is important to ensure that something
    from files that call VIDE_REGISTER_TYPE is odr-used before serialization
    occurs, otherwise the registration will never take place.  This may often
    be the case when serialization is built as a shared library external from
    your main program.

    This macro, with any name of your choosing, should be placed into the
    source file that contains calls to VIDE_REGISTER_TYPE.

    Its counterpart, VIDE_FORCE_DYNAMIC_INIT, should be placed in its
    associated header file such that it is included in the translation units
    (source files) in which you want the registration to appear.

    @relates VIDE_FORCE_DYNAMIC_INIT
    */
#define VIDE_REGISTER_DYNAMIC_INIT(LibName)                \
	namespace vide::detail {                               \
	void VIDE_DLL_EXPORT dynamic_init_dummy_##LibName() {} \
	} /* namespace vide::detail */

//! Forces dynamic initialization of polymorphic support in a
//! previously registered source file
/*! @sa VIDE_REGISTER_DYNAMIC_INIT

    See VIDE_REGISTER_DYNAMIC_INIT for detailed explanation
    of how this macro should be used.  The name used should
    match that for VIDE_REGISTER_DYNAMIC_INIT. */
#define VIDE_FORCE_DYNAMIC_INIT(LibName)                    \
	namespace vide::detail {                                \
	void VIDE_DLL_EXPORT dynamic_init_dummy_##LibName();    \
	} /* namespace vide::detail */                          \
	namespace {                                             \
	struct dynamic_init_##LibName {                         \
		dynamic_init_##LibName() {                          \
			::vide::detail::dynamic_init_dummy_##LibName(); \
		}                                                   \
	} dynamic_init_instance_##LibName;                      \
	} /* namespace */


namespace vide::polymorphic_detail { // ------------------------------------------------------------

//! Error message used for unregistered polymorphic types
/*! @internal */
#define UNREGISTERED_POLYMORPHIC_EXCEPTION(LoadSave, Name, ArName)                                                                              \
      throw vide::Exception("Trying to " LoadSave " an unregistered polymorphic type: " + Name + " with " + ArName + ".\n"                      \
                              "Make sure your type is registered with VIDE_REGISTER_TYPE and that the archive "                                 \
                              "you are using was included (and registered with VIDE_REGISTER_ARCHIVE) prior to calling VIDE_REGISTER_TYPE.\n"   \
                              "If your type is already registered and you still see this error, you may need to use VIDE_REGISTER_DYNAMIC_INIT.");

//! Get an input binding from the given archive by deserializing the type metadata
/*! @internal */
template <class Archive>
inline ::vide::polymorphic_detail::InputSerializers aux_getPolymorphicInputSerializer(Archive& ar, const std::string& polymorphic_name) {
	const auto& bindingMap = detail::getBindingMapInput<Archive>();
	auto it = bindingMap.find(polymorphic_name);
	if (it == bindingMap.end())
		UNREGISTERED_POLYMORPHIC_EXCEPTION("load", polymorphic_name, vide::util::demangle(typeid(ar).name()))
	return it->second;
}

template <class Archive>
inline ::vide::polymorphic_detail::InputSerializers getPolymorphicInputSerializer(Archive& ar, const std::string& polymorphic_name) {
	if constexpr (Archive::is_proxy)
		return aux_getPolymorphicInputSerializer(ar.underlying(), polymorphic_name);
	else
		return aux_getPolymorphicInputSerializer(ar, polymorphic_name);
}

} // namespace vide::polymorphic_detail ===========================================================
namespace vide { // -------------------------------------------------------------------------------

/// Saving std::shared_ptr for polymorphic types
template <class Archive, class T>
		requires std::is_polymorphic_v<T>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::shared_ptr<T>& var) {
	if (!var) {
		ar.nvp("ref", std::uint32_t{0});
		return;
	}

	const std::type_info& objectType = typeid(*var);
	static const std::type_info& savingType = typeid(T);

	// objectType can never match savingType if T is abstract (can't have an instance of an abstract class)
	if constexpr (!std::is_abstract_v<T>) {
		if (objectType == savingType) {
			// Matching type_info means this pointer does not need to be cast with our polymorphic machinery
			// as the current T (static type) matched the variable real (dynamic) type. We indicate this with
			// a zero as polymorphic_id.
			const auto objectAddress = var.get();
			const auto [stored, ref, new_] = ar.registerSmartPointer(objectAddress);
			ar.nvp("ref", ref);
			if (!new_)
				return;

			stored.assign(var);
			ar.nvp("polymorphic_id", polymorphic_id_t{0});
			ar.nvp("data", *var);
			return;
		}
	}

	const auto& bindingMap = detail::getBindingMapOutput<Archive>();
	auto binding = bindingMap.find(std::type_index(objectType));
	if (binding == bindingMap.end()) {
		if constexpr (std::is_abstract_v<T>)
			UNREGISTERED_POLYMORPHIC_EXCEPTION("save [polymorphic abstract]", vide::util::demangle(objectType.name()), vide::util::demangle(typeid(ar).name()))
		else
			UNREGISTERED_POLYMORPHIC_EXCEPTION("save [polymorphic not abstract]", vide::util::demangle(objectType.name()), vide::util::demangle(typeid(ar).name()))
	}

	const auto objectAddress = binding->second.downcast(var.get(), savingType);
	const auto [stored, ref, new_] = ar.registerSmartPointer(objectAddress);

	ar.nvp("ref", ref);
	if (!new_)
		return;

	stored.assign(var);
	binding->second.generic_ptr(&to_underlying_ar(ar), objectAddress);
}

/// Loading std::shared_ptr for polymorphic types
template <class Archive, class T>
		requires std::is_polymorphic_v<T>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::shared_ptr<T>& var) {
	using NonConstT = std::remove_const_t<T>;
	static const std::type_info& loadingType = typeid(T);

	std::uint32_t ref;
	ar.nvp("ref", ref);
	if (ref == 0) {
		var = nullptr;
		return;
	}

	const auto [stored, new_] = ar.registerSmartPointer(ref);
	if (new_) {
		polymorphic_id_t polymorphic_id;
		ar.nvp("polymorphic_id", polymorphic_id);

		if (polymorphic_id == 0) {
			// Zero polymorphic_id means this pointer does not need to be cast with our polymorphic machinery
			// as the current T (static type) matched the variable real (dynamic) type

			if constexpr (std::is_abstract_v<T> || !traits::is_default_constructible<T>) {
				// This case is for when we can't actually construct the shared pointer. Normally this would be caught
				// as the pointer itself is serialized, but since this is a polymorphic pointer, if we tried to serialize
				// the pointer we'd end up back here recursively.  So we have to catch the error here as well, if
				// this was a polymorphic type serialized by its proper pointer type
				throw vide::Exception("Cannot load a polymorphic type '" + vide::util::demangle(loadingType.name()) + "' that is not default constructable");
			} else {
				std::shared_ptr<NonConstT> ptr(::vide::access::construct<NonConstT>());
				NonConstT* objectAddress = ptr.get();
				stored.assign(std::shared_ptr<void>(ptr), loadingType, &type_tag_std_shared_ptr,
						+[](void* varPtr, const std::type_info& futureLoadingType) -> void* {
							return detail::PolymorphicCasters::upcast<NonConstT>(static_cast<NonConstT*>(varPtr), futureLoadingType);
						});
				var = std::move(ptr);
				ar.nvp("data", *objectAddress);
				return;
			}
		} else {
			auto [serializer, new_] = ar.registerPolymorphicType(polymorphic_id);
			if (new_) {
				std::string polymorphic_name;
				ar.nvp("polymorphic_name", polymorphic_name);
				serializer = polymorphic_detail::getPolymorphicInputSerializer(ar, polymorphic_name);
			}

			serializer.generic_ptr(&to_underlying_ar(ar), loadingType, [&](void* objectAddress, void* varPtr, const std::type_index& objectType) {
				// Info and upcast must be set before the actual serialization
				// registerFn takes ownership of the loaded pointer
				// We create the shared_ptr by pointing it to varPtr, but with aliasing constructor we store
				// the real (most downcasted / dynamic type) ptr.
				var = std::shared_ptr<NonConstT>(static_cast<NonConstT*>(varPtr));
				auto aliasedPtr = std::shared_ptr<void>(var, objectAddress); // Aliasing constructor
				stored.assign(std::move(aliasedPtr), objectType, &type_tag_std_shared_ptr, serializer.upcast);
			});
			return;
		}
	}

	if (stored.pointerType != &type_tag_std_shared_ptr)
		throw Exception(
				"Type mismatch. Polymorphic pointer type '" + std::string(type_tag_std_shared_ptr.name) + "' referenced with ref [" + std::to_string(ref) + "] was previously loaded as a different '" +
				std::string(stored.pointerType->name) + "' pointer type.");

	if (stored.objectType == loadingType) {
		std::shared_ptr<void> tmp;
		stored.copyTo(&tmp);
		var = std::static_pointer_cast<T>(tmp);
	} else {
		if (stored.upcast == nullptr)
			throw Exception(
					"Type mismatch. Polymorphic '" + std::string(type_tag_std_shared_ptr.name) + "' referenced with ref [" + std::to_string(ref) + "] was previously loaded as non-polymorphic type '" +
					util::demangle(stored.objectType.name()) + "', but now it is requested as polymorphic type '" + util::demangledName<T>() + "'.");

		std::shared_ptr<void> tmp;
		stored.copyTo(&tmp);
		var = std::shared_ptr<T>(tmp, static_cast<NonConstT*>(stored.upcast(tmp.get(), loadingType))); // Aliasing constructor
	}
}

// -------------------------------------------------------------------------------------------------

/// Saving std::unique_ptr for polymorphic types
template <class Archive, class T, class D>
	requires std::is_polymorphic_v<T>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::unique_ptr<T, D>& var) {
	// unique_ptr gets one bool of metadata which signifies whether they were a nullptr
	ar.nvp("valid", var != nullptr);
	if (var == nullptr)
		return;

	const std::type_info& objectType = typeid(*var);
	static const std::type_info& savingType = typeid(T);

	// objectType can never match savingType if T is abstract (can't have an instance of an abstract class)
	if constexpr (!std::is_abstract_v<T>) {
		if (objectType == savingType) {
			// Matching type_info means this pointer does not need to be cast with our polymorphic machinery
			// as the current T (static type) matched the variable real (dynamic) type. We indicate this with
			// a zero as polymorphic_id.
			ar.nvp("polymorphic_id", polymorphic_id_t{0});
			ar.nvp("data", *var);
			return;
		}
	}

	const auto& bindingMap = detail::getBindingMapOutput<Archive>();

	auto binding = bindingMap.find(std::type_index(objectType));
	if (binding == bindingMap.end()) {
		if constexpr (std::is_abstract_v<T>)
			UNREGISTERED_POLYMORPHIC_EXCEPTION("save [polymorphic abstract]", vide::util::demangle(objectType.name()), vide::util::demangle(typeid(ar).name()))
		else
			UNREGISTERED_POLYMORPHIC_EXCEPTION("save [polymorphic not abstract]", vide::util::demangle(objectType.name()), vide::util::demangle(typeid(ar).name()))
	}

	const auto objectAddress = binding->second.downcast(var.get(), savingType);
	binding->second.generic_ptr(&to_underlying_ar(ar), objectAddress);
}

/// Loading std::unique_ptr for polymorphic types
template <class Archive, class T, class D>
		requires std::is_polymorphic_v<T>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::unique_ptr<T, D>& var) {
	bool valid;
	ar.nvp("valid", valid);

	if (!valid) {
		var = nullptr;
		return;
	}

	static const std::type_info& loadingType = typeid(T);
	polymorphic_id_t polymorphic_id;
	ar.nvp("polymorphic_id", polymorphic_id);

	if (polymorphic_id == 0) {
		// Zero polymorphic_id means this pointer does not need to be cast with our polymorphic machinery
		// as the current T (static type) matched the variable real (dynamic) type

		if constexpr (std::is_abstract_v<T> || !traits::is_default_constructible<T>) {
			// This case is for when we can't actually construct the shared pointer. Normally this would be caught
			// as the pointer itself is serialized, but since this is a polymorphic pointer, if we tried to serialize
			// the pointer we'd end up back here recursively.  So we have to catch the error here as well, if
			// this was a polymorphic type serialized by its proper pointer type
			throw vide::Exception("Cannot load a polymorphic type '" + vide::util::demangle(loadingType.name()) + "' that is not default constructable");
		} else {
			using NonConstT = std::remove_const_t<T>;
			std::unique_ptr<NonConstT, D> ptr(::vide::access::construct<NonConstT>());
			ar.nvp("data", *ptr);
			var = std::move(ptr);
		}
	} else {
		auto [serializer, new_] = ar.registerPolymorphicType(polymorphic_id);
		if (new_) {
			std::string polymorphic_name;
			ar.nvp("polymorphic_name", polymorphic_name);
			serializer = polymorphic_detail::getPolymorphicInputSerializer(ar, polymorphic_name);
		}

		const auto registerFn = [&var](void* objectAddress, void* varPtr, const std::type_index& objectType) {
			// registerFn takes ownership of the loaded pointer
			(void) objectAddress;
			(void) objectType;
			var = std::unique_ptr<T, D>(static_cast<T*>(varPtr));
		};
		serializer.generic_ptr(&to_underlying_ar(ar), loadingType, registerFn);
	}
}

} // namespace vide --------------------------------------------------------------------------------

#undef UNREGISTERED_POLYMORPHIC_EXCEPTION
