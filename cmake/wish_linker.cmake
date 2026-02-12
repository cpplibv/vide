#

include_guard(GLOBAL)


# -------------------------------------------------------------------------------------------------

function(wish_alternative_linker linker_name)
	if (NOT linker_name)
		return()
	endif ()

	find_program(linker_executable ld.${linker_name} ${linker_name})
	if (linker_executable)
		if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 12.0.0)
			add_link_options("-ld-path=${linker_name}")
		else ()
			add_link_options("-fuse-ld=${linker_name}")
		endif ()
		message(STATUS "Wish: Alternative linker: ${linker_name}")
	else ()
		message(FATAL_ERROR "Wish: Could not enable alternative linker: ${linker_name} program was not found")
	endif ()
endfunction()

# -------------------------------------------------------------------------------------------------

include(CheckIPOSupported)

function(wish_enable_ipo enable)
	if (NOT enable)
		return()
	endif ()

	check_ipo_supported(RESULT result OUTPUT output)
	if (result)
		set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE PARENT_SCOPE)
		message(STATUS "Wish: IPO enabled")
	else ()
		message(WARNING "Wish: IPO is not supported: ${output}")
	endif ()
endfunction()

# -------------------------------------------------------------------------------------------------
