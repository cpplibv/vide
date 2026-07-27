#pragma once

#include <vide/macros.hpp>

#include <chrono>


namespace vide { // --------------------------------------------------------------------------------

// --- std::chrono::duration ---

template <typename Archive, typename Rep, typename Period>
inline Rep VIDE_FUNCTION_NAME_SAVE_MINIMAL(Archive&, const std::chrono::duration<Rep, Period>& duration) {
	return duration.count();
}

template <typename Archive, typename Rep, typename Period>
inline void VIDE_FUNCTION_NAME_LOAD_MINIMAL(Archive&, std::chrono::duration<Rep, Period>& duration, Rep count) {
	duration = std::chrono::duration<Rep, Period>{count};
}

// --- std::chrono::time_point ---

template <typename Archive, typename Clock, typename Duration>
inline typename Duration::rep VIDE_FUNCTION_NAME_SAVE_MINIMAL(Archive&, const std::chrono::time_point<Clock, Duration>& time_point) {
	return time_point.time_since_epoch().count();
}

template <typename Archive, typename Clock, typename Duration>
inline void VIDE_FUNCTION_NAME_LOAD_MINIMAL(Archive&, std::chrono::time_point<Clock, Duration>& time_point, typename Duration::rep count) {
	time_point = std::chrono::time_point<Clock, Duration>{Duration{count}};
}

} // namespace vide --------------------------------------------------------------------------------

