#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <vide/details/polymorphic_helper.hpp>
#include <vide/details/stored_smart_ptr.hpp>
#include <vide/details/util.hpp>
#include <vide/details/validation.hpp>
#include <vide/details/vide_types.hpp>
#include <vide/exception.hpp>
#include <vide/macros.hpp>


namespace vide::detail { // -----------------------------------------------------------------------

/// One base class subobject: its type and address
struct base_class_id {
	std::type_index type;
	const void* ptr;
	std::size_t hash;

	template <class T>
	explicit base_class_id(const T* const t) :
		type(typeid(T)),
		ptr(t),
		hash(util::hash_combine(std::hash<std::type_index>{}(type), ptr)) {
	}

	bool operator==(base_class_id const& other) const noexcept {
		return type == other.type && ptr == other.ptr;
	}
};

struct base_class_id_hash {
	std::size_t operator()(base_class_id const& id) const noexcept {
		return id.hash;
	}
};

// =================================================================================================

// Non-template bases of OutputArchive and InputArchive
/* They hold the bookkeeping that ignores the archive type: one copy of its code per TU, not one
   per ArchiveType. rtti() only exists to let an archive be used polymorphically, see the
   archive adapters */

/// Bookkeeping and validator providers shared by both directions
class ArchiveBase {
	/// Virtual bases already serialized; each is serialized once
	std::unordered_set<base_class_id, base_class_id_hash> serializedVirtualBaseClasses;

	std::vector<std::function<void()>> deferments;

public:
	ArchiveBase() = default;
	ArchiveBase(ArchiveBase&&) noexcept = default;
	ArchiveBase& operator=(ArchiveBase&&) noexcept = default;
	virtual ~ArchiveBase() noexcept = default;

private:
	virtual void rtti() {}

public:
	// Dependent name providers:
	using Exception = vide::Exception;

	[[no_unique_address]] notnull_t notnull;
	[[no_unique_address]] notnullrange_t notnullrange;
	[[no_unique_address]] notempty_t notempty;
	[[nodiscard]] constexpr inline maxsize_t maxsize(std::size_t limit) {
		return maxsize_t{limit};
	}
	[[nodiscard]] constexpr inline minsize_t minsize(std::size_t limit) {
		return minsize_t{limit};
	}
	template <typename Base>
	[[nodiscard]] constexpr inline auto indirect(Base&& baseValidator) {
		return indirect_t<std::remove_cvref_t<Base>>{std::forward<Base>(baseValidator)};
	}
	[[nodiscard]] constexpr inline auto indirect_maxsize(std::size_t limit) {
		return indirect_t<maxsize_t>{limit};
	}
	template <typename Base>
	[[nodiscard]] constexpr inline auto ranged(Base&& baseValidator) {
		return ranged_t<std::remove_cvref_t<Base>>{std::forward<Base>(baseValidator)};
	}

public:
	/// Serializes the data wrapped in DeferredData so far
	// Template and VIDE_NOINLINE solely for compile time
	template <class = void>
	VIDE_NOINLINE void serializeDeferments() {
		// Indexes, not iterators: a deferment may add further deferments
		for (std::size_t i = 0; i < deferments.size(); i++)
			deferments[i]();
	}

	/// Default case, does nothing
	inline void setNextName(const char* name) {
		(void) name;
	}

protected:
	/// True on the first registration of id since the last reset()
	// Template and VIDE_NOINLINE solely for compile time
	template <class = void>
	VIDE_NOINLINE bool registerVirtualBaseClass(const base_class_id& id) {
		return serializedVirtualBaseClasses.insert(id).second;
	}

	// Template and VIDE_NOINLINE solely for compile time
	template <class = void>
	VIDE_NOINLINE void addDeferment(std::function<void()> deferment) {
		deferments.emplace_back(std::move(deferment));
	}

	// Template solely for compile time
	template <class = void>
	void reset() {
		serializedVirtualBaseClasses.clear();
		deferments.clear();
	}
};

// =================================================================================================

class OutputArchiveBase : public ArchiveBase {
	/// Object address -> pointer ref id. Keeps a copy of each smart pointer alive, so a freed
	/// address cannot be reused by another object mid serialization (CVE-2020-11105)
	std::unordered_map<const void*, StoredOutputPointer> storedSmartPointerMap;

	/// Polymorphic type name -> id
	std::unordered_map<const char*, polymorphic_id_t> polymorphicTypeMap;
	polymorphic_id_t nextPolymorphicTypeId = 1;

	/// Types whose version is already written
	std::unordered_set<std::type_index> versionedTypes;

public:
	struct RegisterSharedPointerResult {
		StoredOutputPointer& stored;
		std::uint32_t ref = 0;
		bool new_ = false;
	};

	/// Registers a smart pointer target; shared targets get one ref id, restoring their relations on load
	// Template and VIDE_NOINLINE solely for compile time
	template <class = void>
	VIDE_NOINLINE RegisterSharedPointerResult registerSmartPointer(const void* objectAddress) {
		assert(objectAddress != nullptr);
		auto& r = storedSmartPointerMap[objectAddress];
		const auto isNew = !r.isAssigned();
		if (isNew)
			r.ref = static_cast<std::uint32_t>(storedSmartPointerMap.size());
		return {r, r.ref, isNew};
	}

	struct RegisterPolymorphicTypeResult {
		polymorphic_id_t polymorphic_id = 0;
		bool new_ = false;
	};

	/// Id of a polymorphic type name; the name itself is written only on its first use
	// Template and VIDE_NOINLINE solely for compile time
	template <class = void>
	VIDE_NOINLINE RegisterPolymorphicTypeResult registerPolymorphicType(const char* name) {
		auto id = polymorphicTypeMap.find(name);
		if (id != polymorphicTypeMap.end())
			return {id->second, false};

		const auto polymorphic_id = nextPolymorphicTypeId++;
		polymorphicTypeMap.insert({name, polymorphic_id});
		return {polymorphic_id, true};
	}

protected:
	/// True on the first registration of type since the last reset()
	// Template and VIDE_NOINLINE solely for compile time
	template <class = void>
	VIDE_NOINLINE bool registerVersionedType(std::type_index type) {
		return versionedTypes.insert(type).second;
	}

	// Template solely for compile time
	template <class = void>
	void reset() {
		ArchiveBase::reset();
		storedSmartPointerMap.clear();
		polymorphicTypeMap.clear();
		nextPolymorphicTypeId = 1;
		versionedTypes.clear();
	}
};

// -------------------------------------------------------------------------------------------------

class InputArchiveBase : public ArchiveBase {
	/// Default value Until the archive sets its own: cheap enough to lose to a forged size tag,
	/// still spares the reallocations of small containers
	static constexpr std::size_t defaultReserveMemoryBudget = 64 * 1024;

	/// Pointer ref id -> loaded pointer. Entries are used by address: relies on the node stability of unordered_map
	std::unordered_map<std::uint32_t, StoredInputPointer> storedSmartPointerMap;

	std::unordered_map<polymorphic_id_t, polymorphic_detail::InputSerializers> polymorphicTypeMap;

	/// Type -> version read for it
	std::unordered_map<std::type_index, std::uint32_t> versionedTypes;

protected:
	/// Bytes deserialization may reserve up front, before the data proves its size. Bounded on
	/// purpose: one malicious size tag could claim all memory. Archives set it, 2-8x the raw
	/// data size recommended
	std::size_t reserveMemoryBudget = defaultReserveMemoryBudget;

public:
	struct RegisterSmartPointerResult {
		StoredInputPointer& stored;
		bool new_ = false;
	};

	/// Entry of a pointer ref id read from the archive; new_ until the caller fills it
	// Template and VIDE_NOINLINE solely for compile time
	template <class = void>
	VIDE_NOINLINE RegisterSmartPointerResult registerSmartPointer(const std::uint32_t ref) {
		assert(ref != 0);
		auto& stored = storedSmartPointerMap[ref];
		return {stored, stored.pointerType == nullptr};
	}

	struct RegisterPolymorphicTypeResult {
		polymorphic_detail::InputSerializers& serializer;
		bool new_ = false;
	};

	/// Serializers of a polymorphic id; on new_ the caller initializes them
	// Template and VIDE_NOINLINE solely for compile time
	template <class = void>
	VIDE_NOINLINE RegisterPolymorphicTypeResult registerPolymorphicType(polymorphic_id_t polymorphic_id) {
		auto r = polymorphicTypeMap.emplace(polymorphic_id, polymorphic_detail::InputSerializers{nullptr, nullptr});
		return {r.first->second, r.second};
	}

protected:
	/// Version read earlier for type, nullptr before its first load
	// Template and VIDE_NOINLINE solely for compile time
	template <class = void>
	VIDE_NOINLINE const std::uint32_t* findLoadedVersion(std::type_index type) const {
		const auto it = versionedTypes.find(type);
		return it != versionedTypes.end() ? &it->second : nullptr;
	}

	// Template and VIDE_NOINLINE solely for compile time
	template <class = void>
	VIDE_NOINLINE void addLoadedVersion(std::type_index type, std::uint32_t version) {
		versionedTypes.emplace(type, version);
	}

	// Template solely for compile time
	template <class = void>
	void reset() {
		ArchiveBase::reset();
		storedSmartPointerMap.clear();
		polymorphicTypeMap.clear();
		versionedTypes.clear();
		reserveMemoryBudget = defaultReserveMemoryBudget;
	}
};

} // namespace vide::detail ------------------------------------------------------------------------
