cmake_minimum_required(VERSION 3.24)

project(vide VERSION 4.0.0 LANGUAGES CXX)

option(VIDE_BUILD_DOC "Build documentation" ${PROJECT_IS_TOP_LEVEL})
option(VIDE_BUILD_SANDBOX "Build sandbox examples" ${PROJECT_IS_TOP_LEVEL})
option(VIDE_BUILD_TESTS "Build tests" ${PROJECT_IS_TOP_LEVEL})
option(VIDE_INSTALL "Generate the install target" ${PROJECT_IS_TOP_LEVEL})
option(VIDE_JUST_INSTALL "Don't do anything besides installing the library" OFF)

# --- Library --------------------------------------------------------------------------------------

add_library(vide INTERFACE)
add_library(vide::vide ALIAS vide)

file(GLOB_RECURSE vide_headers CONFIGURE_DEPENDS include/*.hpp)
target_sources(vide INTERFACE FILE_SET HEADERS BASE_DIRS include FILES ${vide_headers})

# Bundled rapidjson headers and licenses: installed, but not verified (some header are MSVC only)
file(GLOB_RECURSE vide_external_files CONFIGURE_DEPENDS include/vide/external/*)
list(FILTER vide_external_files EXCLUDE REGEX "\\.hpp$")
target_sources(vide INTERFACE FILE_SET external TYPE HEADERS BASE_DIRS include FILES ${vide_external_files})
target_compile_features(vide INTERFACE cxx_std_26)
# Every header relies on static reflection, so consumers need the flag as well
target_compile_options(vide INTERFACE -freflection)

# --- Install --------------------------------------------------------------------------------------

if (VIDE_INSTALL)
	include(GNUInstallDirs)
	include(CMakePackageConfigHelpers)

	# Header-only, so the package files are arch independent and go to share/
	set(vide_config_dir ${CMAKE_INSTALL_DATADIR}/cmake/vide)

	install(TARGETS vide EXPORT videTargets FILE_SET HEADERS FILE_SET external)
	install(EXPORT videTargets NAMESPACE vide:: DESTINATION ${vide_config_dir})

	configure_package_config_file(Config.cmake.in videConfig.cmake INSTALL_DESTINATION ${vide_config_dir})
	write_basic_package_version_file(videConfigVersion.cmake COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
	install(FILES
			${CMAKE_CURRENT_BINARY_DIR}/videConfig.cmake
			${CMAKE_CURRENT_BINARY_DIR}/videConfigVersion.cmake
			DESTINATION ${vide_config_dir})
endif ()

# --- Development ----------------------------------------------------------------------------------

if (PROJECT_IS_TOP_LEVEL)
	if (NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
	endif ()
	# Dev: optimized, but asserts stay enabled
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release Dev)

	set_target_properties(vide PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON INTERFACE_HEADER_SETS_TO_VERIFY HEADERS)

	add_compile_options(
			-ftemplate-backtrace-limit=100
			$<$<CONFIG:Debug>:-ggdb3>
			$<$<CONFIG:Dev>:-O3>
	)
	# Executables only; -static on a shared library breaks its link
	add_link_options("$<$<AND:$<CONFIG:Release>,$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>>:-static>")

	set(vide_gnu_warnings
			-Wall
			-Warray-bounds
#			-Wcast-align=strict # rapidxml / rapidjson has a bunch of these
			-Wcast-qual
#			-Wconversion # Too many, but should cleanup all
			-Wdelete-non-virtual-dtor
			-Wdouble-promotion
			-Wduplicated-branches
			-Wduplicated-cond
			-Wextra
			-Wlogical-op
			-Wmultistatement-macros
			-Wnon-virtual-dtor
#			-Wnull-dereference # Too many false positive
			-Wold-style-cast
			-Wpedantic
			-Wrestrict
			-Wshadow-compatible-local
			-Wsuggest-override
			-Wundef
			-Wno-comment
	)
	set(vide_clang_warnings
			-Weverything
			-Wmissing-override
#			-Wconversion
			-Wno-comment
	)
	add_compile_options(
			"$<$<CXX_COMPILER_ID:GNU>:${vide_gnu_warnings}>"
			"$<$<CXX_COMPILER_ID:Clang,AppleClang>:${vide_clang_warnings}>"
			"$<$<CXX_COMPILER_ID:MSVC>:/Wall>"
	)
endif ()

# --- Subprojects ----------------------------------------------------------------------------------

if (VIDE_JUST_INSTALL)
	return()
endif ()

if (VIDE_BUILD_TESTS)
	enable_testing()
	add_subdirectory(unittests)
endif ()

if (VIDE_BUILD_SANDBOX)
	add_subdirectory(sandbox)
endif ()

if (VIDE_BUILD_DOC)
	add_subdirectory(doc)
endif ()
