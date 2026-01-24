//

#pragma once

#include <type_traits>



namespace my { // ----------------------------------------------------------------------------------

constexpr inline vide::SmartPtrTag type_tag_my_intrusive_ptr{"my::intrusive_ptr"};

template <typename T>
struct intrusive_ptr {
	T* ptr = nullptr;

	intrusive_ptr() = default;

	explicit intrusive_ptr(T* p) :
		ptr(p) {
		if (ptr)
			++ptr->ref_count;
	}

	intrusive_ptr(const intrusive_ptr& other) :
		ptr(other.ptr) {
		if (ptr)
			++ptr->ref_count;
	}

	intrusive_ptr(intrusive_ptr&& other) :
		ptr(other.ptr) {
		other.ptr = nullptr;
	}

	template <typename K>
			requires std::is_convertible_v<K*, T*>
	intrusive_ptr(const intrusive_ptr<K>& other) :
		ptr(other.ptr) {
		if (ptr)
			++ptr->ref_count;
	}

	template <typename K>
			requires std::is_convertible_v<K*, T*>
	intrusive_ptr(intrusive_ptr<K>&& other) :
		ptr(other.ptr) {
		other.ptr = nullptr;
	}

	intrusive_ptr& operator=(const intrusive_ptr& other) {
		if (ptr)
			if (--ptr->ref_count == 0)
				delete ptr;

		ptr = other.ptr;
		if (ptr)
			++ptr->ref_count;
		return *this;
	}

	intrusive_ptr& operator=(intrusive_ptr&& other) {
		if (ptr)
			if (--ptr->ref_count == 0)
				delete ptr;

		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}

	template <typename K>
			requires std::is_convertible_v<K*, T*>
	intrusive_ptr& operator=(const intrusive_ptr<K>& other) {
		if (ptr)
			if (--ptr->ref_count == 0)
				delete ptr;

		ptr = other.ptr;
		if (ptr)
			++ptr->ref_count;
		return *this;
	}

	template <typename K>
			requires std::is_convertible_v<K*, T*>
	intrusive_ptr& operator=(intrusive_ptr<K>&& other) {
		if (ptr)
			if (--ptr->ref_count == 0)
				delete ptr;

		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}

	~intrusive_ptr() {
		if (ptr)
			if (--ptr->ref_count == 0)
				delete ptr;
	}

	intrusive_ptr& operator=(std::nullptr_t) {
		if (ptr) {
			if (--ptr->ref_count == 0)
				delete ptr;
			ptr = nullptr;
		}
		return *this;
	}

	explicit operator bool() const noexcept {
		return ptr != nullptr;
	}

	T& operator*() const noexcept {
		return *ptr;
	}

	[[nodiscard]] T* get() const noexcept {
		return ptr;
	}
};

/// Saving my::intrusive_ptr for non-polymorphic types
template <class Archive, class T>
		requires (!std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const my::intrusive_ptr<T>& var) {
	if (!var) {
		ar.nvp("ref", std::uint32_t{0});
		return;
	}

	const auto* objectAddress = var.get();
	const auto [stored, ref, new_] = ar.registerSmartPointer(objectAddress);
	ar.nvp("ref", ref);
	if (new_) {
		stored.assign(var);
		ar.nvp("data", *objectAddress);
	}
}

/// Loading my::intrusive_ptr for non-polymorphic types
template <class Archive, class T>
		requires (!std::is_polymorphic_v<T>)
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, my::intrusive_ptr<T>& var) {
	std::uint32_t ref;
	ar.nvp("ref", ref);
	if (ref == 0) {
		var = nullptr;
		return;
	}

	const auto [stored, new_] = ar.registerSmartPointer(ref);
	if (new_) {
		using NonConstT = std::remove_const_t<T>;
		my::intrusive_ptr<NonConstT> ptr(::vide::access::construct<NonConstT>());
		NonConstT* objectAddress = ptr.get();
		stored.assign(ptr, typeid(T), &type_tag_my_intrusive_ptr);
		var = std::move(ptr);
		ar.nvp("data", *objectAddress);
		return;
	}

	if (stored.pointerType != &type_tag_my_intrusive_ptr)
		throw vide::Exception(
				"Type mismatch. Non-polymorphic pointer type '" + std::string(type_tag_my_intrusive_ptr.name) + "' referenced with ref [" + std::to_string(ref) + "] was previously loaded as a different '" +
				std::string(stored.pointerType->name) + "' pointer type.");

	if (stored.objectType != typeid(T)) // This check also handles any polymorphic mismatch
		throw vide::Exception(
				"Type mismatch. Non-polymorphic '" + std::string(type_tag_my_intrusive_ptr.name) + "' referenced with ref [" + std::to_string(ref) + "] was previously loaded as type '" +
				vide::util::demangle(stored.objectType.name()) + "' is now requested as type '" + vide::util::demangledName<T>() + "'.");

	stored.copyTo(&var);
}

} // namespace my ----------------------------------------------------------------------------------

struct TestRefCounted {
	mutable int ref_count = 0;
	int value = 0;

public:
	TestRefCounted() = default;
	explicit TestRefCounted(int value) : value(value) {
	}

	template <typename Archive>	void serialize(Archive& ar) {
		ar.nvp("value", value);
	}

	[[nodiscard]] bool operator==(const TestRefCounted& other) const {
		return value == other.value;
	}
};

// -------------------------------------------------------------------------------------------------
