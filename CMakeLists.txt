cmake_minimum_required(VERSION 3.24)

project(vide LANGUAGES CXX VERSION 3.0.2)

include(cmake/wish_color.cmake)
include(cmake/wish_create.cmake)
include(cmake/wish_linker.cmake)
include(cmake/wish_warning.cmake)


if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RELEASE)
endif ()

set(CMAKE_CONFIGURATION_TYPES "DEBUG;RELEASE;DEV" CACHE STRING "" FORCE)


option(VIDE_BUILD_DOC "Build documentation" ${PROJECT_IS_TOP_LEVEL})
option(VIDE_BUILD_SANDBOX "Build sandbox examples" ${PROJECT_IS_TOP_LEVEL})
option(VIDE_BUILD_TESTS "Build tests" ${PROJECT_IS_TOP_LEVEL})
option(VIDE_JUST_INSTALL "Don't do anything besides installing the library" FALSE)
option(VIDE_SKIP_PERFORMANCE_COMPARISON "Skip building performance sandbox comparison (requires boost)" FALSE)
option(VIDE_INSTALL "Generate the install target" ${PROJECT_IS_TOP_LEVEL})

option(VIDE_FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." FALSE)
wish_force_colored_output(${VIDE_FORCE_COLORED_OUTPUT})
option(VIDE_ALTERNATIVE_LINKER "Use an alternative linker. Leave empty for system default; alternatives are 'gold', 'lld', 'bfd', 'mold'" FALSE)
wish_alternative_linker(${VIDE_ALTERNATIVE_LINKER})


if (PROJECT_IS_TOP_LEVEL) # ---------------------------------------------------------------------------

	wish_warning(
			MSVC /Wall

			Clang -Weverything
			Clang -Wmissing-override
#			Clang -Wconversion

			GNU -Wall
			GNU -Warray-bounds
#			GNU -Wcast-align=strict # rapidxml / rapidjson has a bunch of these
			GNU -Wcast-qual
#			GNU -Wconversion # Too many, but should cleanup all
			GNU -Wdelete-non-virtual-dtor
			GNU -Wdouble-promotion
			GNU -Wduplicated-branches
			GNU -Wduplicated-cond
			GNU -Wextra
			GNU -Wlogical-op
			GNU -Wmultistatement-macros
			GNU -Wnon-virtual-dtor
#			GNU -Wnull-dereference # Too many false positive
			GNU -Wold-style-cast
			GNU -Wpedantic
			GNU -Wrestrict
			GNU -Wshadow-compatible-local
			GNU -Wsuggest-override
			GNU -Wundef

			# Warnings that I don't care about
			Clang -Wno-comment
			GNU -Wno-comment
	)

	# -------------------------------------------------------------------------------------------------

	string(TOLOWER ${CMAKE_BUILD_TYPE} vide_build_type)
	if (vide_build_type STREQUAL "debug")
		#add_compile_options(-Og)
		add_compile_options(-ggdb3)
		#	add_compile_options(-fno-omit-frame-pointer)

	elseif (vide_build_type STREQUAL "release")
		add_compile_options(-O3)
		#	add_compile_options(-flto)
		#	SET(CMAKE_AR "gcc-ar")
		#	SET(CMAKE_NM "gcc-nm")
		#	SET(CMAKE_RANLIB "gcc-ranlib")
		#	add_definitions(-DNDEBUG)
		wish_static_link_std()
		add_compile_options(-static) # Normally wish_static_link_std() would take care of this, but vide is not really using wish

	elseif (vide_build_type STREQUAL "dev")
		add_compile_options(-O3)
		#    add_compile_options(-ftemplate-backtrace-limit=0)

	else ()
		message(WARNING "Failed to identify [${CMAKE_BUILD_TYPE}] as a build type")
	endif ()

	add_compile_options(-m64)

	add_compile_options(-ftemplate-backtrace-limit=50)

endif () # ----------------------------------------------------------------------------------------

set(VIDE_THREAD_LIBS)
if (UNIX)
	option(THREAD_SAFE "Use mutexes to ensure thread safety" FALSE)
	if (THREAD_SAFE)
		message(STATUS "Use mutexes")
		add_definitions(-DVIDE_THREAD_SAFE=1)
		set(VIDE_THREAD_LIBS pthread)
	endif ()
endif ()

add_library(vide INTERFACE)
add_library(vide::vide ALIAS vide)

file(GLOB_RECURSE vide_public_header_files LIST_DIRECTORIES false RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS "include/*.hpp")
target_sources(vide PUBLIC
		FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
		FILES ${vide_public_header_files})
if (PROJECT_IS_TOP_LEVEL)
	set_target_properties(vide PROPERTIES VERIFY_INTERFACE_HEADER_SETS TRUE)
endif ()

list(APPEND VIDE_THREAD_LIBS vide::vide)

target_compile_features(vide INTERFACE cxx_std_23)

if (VIDE_INSTALL)
	include(CMakePackageConfigHelpers)

	install(TARGETS vide
			EXPORT ${PROJECT_NAME}Targets
			FILE_SET public_headers DESTINATION include)
	install(DIRECTORY include/vide DESTINATION include)

	set(configFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake)
	set(versionFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)
	set(configInstallDestination lib/cmake/${PROJECT_NAME})

	configure_package_config_file(
			${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
			${configFile}
			INSTALL_DESTINATION ${configInstallDestination}
	)

	write_basic_package_version_file("${versionFile}" COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
	install(FILES ${configFile} ${versionFile} DESTINATION ${configInstallDestination})
	install(
			EXPORT ${PROJECT_NAME}Targets
			NAMESPACE "vide::"
			DESTINATION ${configInstallDestination}
	)
endif ()


if (NOT VIDE_JUST_INSTALL)
	#if(NOT VIDE_SKIP_PERFORMANCE_COMPARISON)
	#    # Boost serialization for performance sandbox
	#    find_package(Boost REQUIRED COMPONENTS serialization)
	#endif()

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
endif ()


# =================================================================================================
# https://github.com/USCiLab/cereal/commit/b2d68c522bdc3a3fd9ab65c26dbd0aedbc61e0e8
#option(SKIP_PORTABILITY_TEST "Skip portability (32 bit) tests" FALSE)
#include(CheckCXXCompilerFlag)
#set(CMAKE_REQUIRED_FLAGS "-m32")
#check_cxx_compiler_flag("-m32" COMPILER_SUPPORT_M32)
#unset(CMAKE_REQUIRED_FLAGS)
#if (NOT COMPILER_SUPPORT_M32)
#	set(SKIP_PORTABILITY_TEST ON CACHE BOOL "Skip portability (32 bit) tests" FORCE)
#endif ()
