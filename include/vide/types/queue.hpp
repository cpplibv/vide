#pragma once

#include <queue>

#include <vide/macros.hpp>

// The default container for queue is deque, so let's include that too
#include <vide/types/deque.hpp>
// The default comparator for queue is less
#include <vide/types/functional.hpp>


namespace vide { // --------------------------------------------------------------------------------
namespace queue_detail {

//! Allows access to the protected container in queue
/*! @internal */
template <class T, class C>
inline const C& container(const std::queue<T, C>& queue) {
	struct H : public std::queue<T, C> {
		static const C& get(const std::queue<T, C>& q) {
			return q.*(&H::c);
		}
	};

	return H::get(queue);
}

//! Allows access to the protected container in priority queue
/*! @internal */
template <class T, class C, class Comp>
inline const C& container(const std::priority_queue<T, C, Comp>& priority_queue) {
	struct H : public std::priority_queue<T, C, Comp> {
		static const C& get(const std::priority_queue<T, C, Comp>& pq) {
			return pq.*(&H::c);
		}
	};

	return H::get(priority_queue);
}

//! Allows access to the protected comparator in priority queue
/*! @internal */
template <class T, class C, class Comp>
inline const Comp& comparator(const std::priority_queue<T, C, Comp>& priority_queue) {
	struct H : public std::priority_queue<T, C, Comp> {
		static const Comp& get(const std::priority_queue<T, C, Comp>& pq) {
			return pq.*(&H::comp);
		}
	};

	return H::get(priority_queue);
}

} // namespace queue_detail ------------------------------------------------------------------------

//! Saving for std::queue
template <class Archive, class T, class C>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::queue<T, C>& queue) {
	ar.nvp("container", queue_detail::container(queue));
}

//! Loading for std::queue
template <class Archive, class T, class C>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::queue<T, C>& queue) {
	C container;
	ar.nvp("container", container);
	queue = std::queue<T, C>(std::move(container));
}

//! Saving for std::priority_queue
template <class Archive, class T, class C, class Comp>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, const std::priority_queue<T, C, Comp>& priority_queue) {
	ar.nvp("comparator", queue_detail::comparator(priority_queue));
	ar.nvp("container", queue_detail::container(priority_queue));
}

//! Loading for std::priority_queue
template <class Archive, class T, class C, class Comp>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::priority_queue<T, C, Comp>& priority_queue) {
	Comp comparator;
	ar.nvp("comparator", comparator);

	C container;
	ar.nvp("container", container);

	priority_queue = std::priority_queue<T, C, Comp>(std::move(comparator), std::move(container));
}

} // namespace vide --------------------------------------------------------------------------------
