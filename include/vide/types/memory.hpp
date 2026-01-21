//

#pragma once

#include <vide/access.hpp>
#include <vide/exception.hpp>
#include <vide/macros.hpp>

#include <memory>
#include <vide/details/util.hpp>


namespace vide { // --------------------------------------------------------------------------------

/// Saving std::shared_ptr for non-polymorphic types
template <class Archive, class T>
		requires (!std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::shared_ptr<T>& var) {
	if (!var) {
		ar.nvp("ref", std::uint32_t{0});
		return;
	}

	const auto [ref, new_] = ar.registerSharedPointer(var);
	ar.nvp("ref", ref);
	if (new_)
		ar.nvp("data", *var);
}

/// Loading std::shared_ptr for non-polymorphic types
template <class Archive, class T>
		requires (!std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::shared_ptr<T>& var) {
	std::uint32_t ref;
	ar.nvp("ref", ref);
	if (ref == 0) {
		var = nullptr;
		return;
	}

	const auto [stored, new_] = ar.registerSharedPointer(ref);
	if (new_) {
		using NonConstT = std::remove_const_t<T>;
		// TODO P2: switch to make_shared_for_overwrite
		//		auto realPtr = std::make_shared<Storage<NonConstT>>();
		//		::vide::access::construct<T>(realPtr.get());
		//		auto ptr = std::shared_ptr<NonConstT>(realPtr, reinterpret_cast<NonConstT>(realPtr.get().data));
		std::shared_ptr<NonConstT> ptr(::vide::access::construct<NonConstT>());
		NonConstT* addr = ptr.get();
		stored.ptr = std::move(ptr);
		stored.info = typeid(T);
		ar.nvp("data", *addr);
		var = std::static_pointer_cast<T>(stored.ptr);
		return;
	}

	if (stored.info != typeid(T))
		throw Exception(
				"Type mismatch. Non-polymorphic shared_ptr referenced by ref [" + std::to_string(ref) + "] was previously loaded as type '" +
				util::demangle(stored.info->name()) + "' is now requested as type '" + util::demangledName<T>() + "'.");

	var = std::static_pointer_cast<T>(stored.ptr);
}

// -------------------------------------------------------------------------------------------------

/// Saving std::unique_ptr for non-polymorphic types
template <class Archive, class T, class D>
		requires (!std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::unique_ptr<T, D>& var) {
	// unique_ptr gets one bool of metadata which signifies whether they were a nullptr
	ar.nvp("valid", var != nullptr);
	if (var == nullptr)
		return;

	ar.nvp("data", *var);
}

/// Loading std::unique_ptr for non-polymorphic types
template <class Archive, class T, class D>
		requires (!std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::unique_ptr<T, D>& var) {
	bool valid;
	ar.nvp("valid", valid);

	if (!valid) {
		var = nullptr;
		return;
	}

	using NonConstT = std::remove_const_t<T>;
	std::unique_ptr<NonConstT, D> ptr(::vide::access::construct<NonConstT>());
	ar.nvp("data", *ptr);
	var = std::move(ptr);
}

// -------------------------------------------------------------------------------------------------

/// Saving std::weak_ptr for both polymorphic and non-polymorphic types as std::shared_ptr
template <class Archive, class T>
inline std::shared_ptr<T> VIDE_FUNCTION_NAME_SAVE_MINIMAL(Archive&, const std::weak_ptr<T>& var) {
	return var.lock();
}

/// Loading std::weak_ptr for both polymorphic and non-polymorphic types as std::shared_ptr
template <class Archive, class T>
inline void VIDE_FUNCTION_NAME_LOAD_MINIMAL(Archive&, std::weak_ptr<T>& var, std::shared_ptr<T>&& value) {
	var = std::move(value);
}

} // namespace vide --------------------------------------------------------------------------------
