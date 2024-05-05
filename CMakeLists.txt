cmake_minimum_required(VERSION 3.20)

project(vide LANGUAGES CXX VERSION 2.3.0)

if (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(VIDE_MASTER_PROJECT ON)
endif ()


include(ExternalProject)
include(cmake/wish_create.cmake)
include(cmake/wish_date.cmake)
include(cmake/wish_git.cmake)
include(cmake/wish_util.cmake)
include(cmake/wish_warning.cmake)


# https://github.com/USCiLab/cereal/commit/b2d68c522bdc3a3fd9ab65c26dbd0aedbc61e0e8
#option(SKIP_PORTABILITY_TEST "Skip portability (32 bit) tests" OFF)
#include(CheckCXXCompilerFlag)
#set(CMAKE_REQUIRED_FLAGS "-m32")
#check_cxx_compiler_flag("-m32" COMPILER_SUPPORT_M32)
#unset(CMAKE_REQUIRED_FLAGS)
#if (NOT COMPILER_SUPPORT_M32)
#	set(SKIP_PORTABILITY_TEST ON CACHE BOOL "Skip portability (32 bit) tests" FORCE)
#endif ()

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RELEASE)
endif ()

set(CMAKE_CONFIGURATION_TYPES "DEBUG;RELEASE;DEV" CACHE STRING "" FORCE)


option(BUILD_DOC "Build documentation" ON)
option(BUILD_SANDBOX "Build sandbox examples" ON)
option(SKIP_PERFORMANCE_COMPARISON "Skip building performance sandbox comparison (requires boost)" OFF)
option(VIDE_INSTALL "Generate the install target" ${VIDE_MASTER_PROJECT})
option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." FALSE)
option(JUST_INSTALL_VIDE "Don't do anything besides installing the library" OFF)


set(VIDE_THREAD_LIBS)
if (UNIX)
	option(THREAD_SAFE "Use mutexes to ensure thread safety" OFF)
	if (THREAD_SAFE)
		message(STATUS "Use mutexes")
		add_definitions(-DVIDE_THREAD_SAFE=1)
		set(VIDE_THREAD_LIBS pthread)
	endif ()
endif ()

if (${FORCE_COLORED_OUTPUT})
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		message(STATUS "Force colored output: GCC")
		add_compile_options(-fdiagnostics-color=always)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		message(STATUS "Force colored output: Clang")
		add_compile_options(-fcolor-diagnostics)
	endif ()
else ()
	message(STATUS "Force colored output: False")
endif ()


#if(MSVC)
#    add_compile_options(/bigobj /W3 /WX)
#else()
#    add_compile_options(-Wall -Wextra -pedantic -Wshadow -Wold-style-cast)
#
##    option(WITH_WERROR "Compile with '-Werror' C++ compiler flag" ON)
##    if(WITH_WERROR)
##        add_compile_options(-Werror)
##    endif()
#
##    option(CLANG_USE_LIBCPP "Use libc++ for clang compilation" OFF)
##    if(APPLE OR CLANG_USE_LIBCPP)
##        message(STATUS "Use libc++")
##        add_compile_options(-stdlib=libc++)
##        # TODO: use add_link_options(-stdlib=libc++ -lc++abi") bud this needs cmake 3.13! CK
##        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
##    endif()
#
##    if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD STREQUAL "98")
##        set(CMAKE_CXX_STANDARD 11)
##    endif()
#
##    set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
#endif()

# -------------------------------------------------------------------------------------------------

wish_warning(
		MSVC /Wall

		Clang -Weverything
		Clang -Wmissing-override
		Clang -Wconversion

		GNU -Wall
		#        GNU -Warray-bounds
		#        GNU -Wcast-align=strict
		#        GNU -Wcast-qual
		#        GNU -Wconversion
		#        GNU -Wdelete-non-virtual-dtor
		#        GNU -Wdouble-promotion
		#        GNU -Wduplicated-branches
		#        GNU -Wduplicated-cond
		GNU -Wextra
		#        GNU -Wlogical-op
		#        GNU -Wmultistatement-macros
		#        GNU -Wnon-virtual-dtor # Causes some false positives with efsw (suppressed)
		#        GNU -Wold-style-cast
		GNU -Wpedantic
		#        GNU -Wrestrict
		#        GNU -Wshadow-compatible-local
		#        GNU -Wsuggest-override
		#        GNU -Wundef

		# Warnings that I don't care about
		Clang -Wno-comment
		GNU -Wno-comment
		# GNU -Wnull-dereference # Causes too many false positives as of GCC 11.2
)

# -------------------------------------------------------------------------------------------------

if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	#add_compile_options(-Og)
	add_compile_options(-ggdb3)
	#	add_compile_options(-fno-omit-frame-pointer)

elseif (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
	add_compile_options(-O3)
	#	add_compile_options(-flto)
	#	SET(CMAKE_AR "gcc-ar")
	#	SET(CMAKE_NM "gcc-nm")
	#	SET(CMAKE_RANLIB "gcc-ranlib")
	#	add_definitions(-DNDEBUG)
	wish_static_link_std()
	add_compile_options(-static) # Normally wish_static_link_std() would take care of this, but vide is not really using wish

elseif (CMAKE_BUILD_TYPE STREQUAL "DEV")
	add_compile_options(-O3)
	#    add_compile_options(-ftemplate-backtrace-limit=0)

else ()
	message(WARNING "Failed to identify [${CMAKE_BUILD_TYPE}] as a build type")
endif ()

add_compile_options(-m64)
add_compile_options(-std=c++23)

# -------------------------------------------------------------------------------------------------

option(BUILD_DOC "Build documentation" ON)
option(BUILD_SANDBOX "Build sandbox examples" ON)
option(SKIP_PERFORMANCE_COMPARISON "Skip building performance sandbox comparison (requires boost)" OFF)
option(VIDE_INSTALL "Generate the install target" ${VIDE_MASTER_PROJECT})
option(VIDE_ALTERNATIVE_LINKER "Use an alternative linker. Leave empty for system default; alternatives are 'gold', 'lld', 'bfd', 'mold'" FALSE)

# -------------------------------------------------------------------------------------------------

function(vide_alternative_linker linker_name)
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
		message(STATUS "Vide: Alternative linker: ${linker_name}")
	else ()
		message(FATAL_ERROR "Vide: Could not enable alternative linker: ${linker_name} program was not found")
	endif ()
endfunction()

vide_alternative_linker(${VIDE_ALTERNATIVE_LINKER})

# -------------------------------------------------------------------------------------------------

add_compile_options(-ftemplate-backtrace-limit=30)

add_library(vide INTERFACE)
add_library(vide::vide ALIAS vide)
target_include_directories(vide INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)
list(APPEND VIDE_THREAD_LIBS vide::vide)

target_compile_features(vide INTERFACE cxx_std_23)

if (VIDE_INSTALL)
	include(CMakePackageConfigHelpers)

	install(TARGETS vide EXPORT ${PROJECT_NAME}Targets)
	install(DIRECTORY include/vide DESTINATION include)

	set(configFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake)
	set(versionFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)
	set(configInstallDestination lib/cmake/${PROJECT_NAME})

	configure_package_config_file(
			${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
			${configFile}
			INSTALL_DESTINATION ${configInstallDestination}
	)
	# https://github.com/USCiLab/cereal/commit/83b6fa06a043e1a9cc4361df79270253439b88ab
	if (${CMAKE_VERSION} VERSION_GREATER 3.13)
		write_basic_package_version_file("${versionFile}" COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
	else ()
		write_basic_package_version_file("${versionFile}" COMPATIBILITY SameMajorVersion)
	endif ()

	install(FILES ${configFile} ${versionFile} DESTINATION ${configInstallDestination})
	install(
			EXPORT ${PROJECT_NAME}Targets
			NAMESPACE "vide::"
			DESTINATION ${configInstallDestination}
	)
endif ()


if (JUST_INSTALL_VIDE)
	return()
endif ()


#if(NOT SKIP_PERFORMANCE_COMPARISON)
#    # Boost serialization for performance sandbox
#    find_package(Boost REQUIRED COMPONENTS serialization)
#endif()


option(BUILD_TESTS "Build tests" ${VIDE_MASTER_PROJECT})
if (BUILD_TESTS)
	enable_testing()
	add_subdirectory(unittests)
endif ()

if (BUILD_SANDBOX)
	add_subdirectory(sandbox)
endif ()

if (BUILD_DOC)
	add_subdirectory(doc)
endif ()
