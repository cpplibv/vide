#pragma once

#include <queue>

#include <vide/macros.hpp>
#include <vide/nvp.hpp>

// The default container for queue is deque, so let's include that too
#include <vide/types/deque.hpp>
// The default comparator for queue is less
#include <vide/types/functional.hpp>


namespace vide { // --------------------------------------------------------------------------------
namespace queue_detail {

//! Allows access to the protected container in queue
/*! @internal */
template <class T, class C>
inline C const& container(std::queue<T, C> const& queue) {
	struct H : public std::queue<T, C> {
		static C const& get(std::queue<T, C> const& q) {
			return q.*(&H::c);
		}
	};

	return H::get(queue);
}

//! Allows access to the protected container in priority queue
/*! @internal */
template <class T, class C, class Comp>
inline C const& container(std::priority_queue<T, C, Comp> const& priority_queue) {
	struct H : public std::priority_queue<T, C, Comp> {
		static C const& get(std::priority_queue<T, C, Comp> const& pq) {
			return pq.*(&H::c);
		}
	};

	return H::get(priority_queue);
}

//! Allows access to the protected comparator in priority queue
/*! @internal */
template <class T, class C, class Comp>
inline Comp const& comparator(std::priority_queue<T, C, Comp> const& priority_queue) {
	struct H : public std::priority_queue<T, C, Comp> {
		static Comp const& get(std::priority_queue<T, C, Comp> const& pq) {
			return pq.*(&H::comp);
		}
	};

	return H::get(priority_queue);
}

} // namespace queue_detail ------------------------------------------------------------------------

//! Saving for std::queue
template <class Archive, class T, class C>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::queue<T, C> const& queue) {
	ar(VIDE_NVP_("container", queue_detail::container( queue )));
}

//! Loading for std::queue
template <class Archive, class T, class C>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::queue<T, C>& queue) {
	C container;
	ar(VIDE_NVP_("container", container));
	queue = std::queue<T, C>(std::move(container));
}

//! Saving for std::priority_queue
template <class Archive, class T, class C, class Comp>
inline void VIDE_FUNCTION_NAME_SAVE(Archive& ar, std::priority_queue<T, C, Comp> const& priority_queue) {
	ar(VIDE_NVP_("comparator", queue_detail::comparator( priority_queue )));
	ar(VIDE_NVP_("container", queue_detail::container( priority_queue )));
}

//! Loading for std::priority_queue
template <class Archive, class T, class C, class Comp>
inline void VIDE_FUNCTION_NAME_LOAD(Archive& ar, std::priority_queue<T, C, Comp>& priority_queue) {
	Comp comparator;
	ar(VIDE_NVP_("comparator", comparator));

	C container;
	ar(VIDE_NVP_("container", container));

	priority_queue = std::priority_queue<T, C, Comp>(comparator, std::move(container));
}

} // namespace vide --------------------------------------------------------------------------------
