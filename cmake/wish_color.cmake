# File: target.cmake, Created on 2017. 04. 14. 16:49, Author: Vader

include_guard(GLOBAL)

# -------------------------------------------------------------------------------------------------

macro(wish_force_colored_output value)
	if (${value})
		message(STATUS "Wish: Force colored output: ON")
		set(CMAKE_COLOR_DIAGNOSTICS ON)
	else ()
		message(STATUS "Wish: Force colored output: OFF")
		set(CMAKE_COLOR_DIAGNOSTICS OFF)
	endif ()
endmacro()

# -------------------------------------------------------------------------------------------------
