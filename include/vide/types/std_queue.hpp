#pragma once

#include <vide/types/functional.hpp> // The default comparator for queue is less
#include <vide/types/std_deque.hpp> // The default container for queue is deque, so let's include that too

#include <cstdint>
#include <queue>


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

template <class T, class C>
struct serializer<std::queue<T, C>> {
	template <class Archive>
	static void serialize(Archive& ar, std::queue<T, C>& queue, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.nvp("container", queue_detail::container(queue));
		} else {
			C container;
			ar.nvp("container", container);
			queue = std::queue<T, C>(std::move(container));
		}
	}
};

template <class T, class C, class Comp>
struct serializer<std::priority_queue<T, C, Comp>> {
	template <class Archive>
	static void serialize(Archive& ar, std::priority_queue<T, C, Comp>& priority_queue, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar.nvp("comparator", queue_detail::comparator(priority_queue));
			ar.nvp("container", queue_detail::container(priority_queue));
		} else {
			Comp comparator;
			ar.nvp("comparator", comparator);

			C container;
			ar.nvp("container", container);

			priority_queue = std::priority_queue<T, C, Comp>(std::move(comparator), std::move(container));
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
