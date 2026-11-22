#pragma once

#include <vide/serializer.hpp>

#include <chrono>
#include <cstdint>


namespace vide { // --------------------------------------------------------------------------------

template <typename Rep, typename Period>
struct serializer<std::chrono::duration<Rep, Period>> {
	static constexpr bool minimal = true;

	template <typename Archive>
	static void serialize(Archive& ar, std::chrono::duration<Rep, Period>& duration, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar(duration.count());
		} else {
			Rep count;
			ar(count);
			duration = std::chrono::duration<Rep, Period>{count};
		}
	}
};

template <typename Clock, typename Duration>
struct serializer<std::chrono::time_point<Clock, Duration>> {
	static constexpr bool minimal = true;

	template <typename Archive>
	static void serialize(Archive& ar, std::chrono::time_point<Clock, Duration>& time_point, std::uint32_t) {
		if constexpr (Archive::is_output) {
			ar(time_point.time_since_epoch().count());
		} else {
			typename Duration::rep count;
			ar(count);
			time_point = std::chrono::time_point<Clock, Duration>{Duration{count}};
		}
	}
};

} // namespace vide --------------------------------------------------------------------------------
