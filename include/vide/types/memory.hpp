//

#pragma once

#include <vide/access.hpp>
#include <vide/macros.hpp>

#include <memory>


namespace vide { // --------------------------------------------------------------------------------
namespace memory_detail {

/// @internal
template <class Archive, class T>
inline void aux_save(Archive& ar, const std::shared_ptr<T>& var) {
	const auto [ref, new_] = ar.registerSharedPointer(var);

	ar.nvp("ref", ref);
	if (new_)
		ar.nvp("data", *var);
}

/// @internal
template <class Archive, class T>
inline void aux_load(Archive& ar, std::shared_ptr<T>& var) {
	uint32_t ref;
	ar.nvp("ref", ref);
	auto [cached, new_] = ar.registerSharedPointer(ref);

	if (new_) {
		using NonConstT = std::remove_const_t<T>;
		// TODO P2: switch to make_shared_for_overwrite
		//		auto realPtr = std::make_shared<Storage<NonConstT>>();
		//		::vide::access::construct<T>(realPtr.get());
		//		auto ptr = std::shared_ptr<NonConstT>(realPtr, reinterpret_cast<NonConstT>(realPtr.get().data));
		std::shared_ptr<NonConstT> ptr(::vide::access::construct<NonConstT>());
		NonConstT* addr = ptr.get();
		*cached = std::move(ptr);
		ar.nvp("data", *addr);
	}

	if (cached)
		var = std::static_pointer_cast<T>(*cached);
	else
		var = nullptr;
}

/// @internal
template <class Archive, class T, class D>
inline void aux_save(Archive& ar, const std::unique_ptr<T, D>& var) {
	// unique_ptr gets one bool of metadata which signifies whether they were a nullptr
	if (var) {
		ar.nvp("valid", true);
		ar.nvp("data", *var);
	} else {
		ar.nvp("valid", false);
	}
}

/// @internal
template <class Archive, class T, class D>
inline void aux_load(Archive& ar, std::unique_ptr<T, D>& var) {
	bool valid;
	ar.nvp("valid", valid);

	if (valid) {
		using NonConstT = std::remove_const_t<T>;
		std::unique_ptr<NonConstT, D> ptr(::vide::access::construct<NonConstT>());
		ar.nvp("data", *ptr);
		var = std::move(ptr);
	} else {
		var.reset(nullptr);
	}
}

} // namespace memory_detail -----------------------------------------------------------------------

/// Saving std::shared_ptr for non-polymorphic types
template <class Archive, class T>
		requires (!std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::shared_ptr<T>& var) {
	memory_detail::aux_save(ar, var);
}

/// Loading std::shared_ptr for non-polymorphic types
template <class Archive, class T>
		requires (!std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::shared_ptr<T>& var) {
	memory_detail::aux_load(ar, var);
}

// -------------------------------------------------------------------------------------------------

/// Saving std::unique_ptr for non-polymorphic types
template <class Archive, class T, class D>
		requires (!std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::unique_ptr<T, D>& var) {
	memory_detail::aux_save(ar, var);
}

/// Loading std::unique_ptr for non-polymorphic types
template <class Archive, class T, class D>
		requires (!std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::unique_ptr<T, D>& var) {
	memory_detail::aux_load(ar, var);
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
