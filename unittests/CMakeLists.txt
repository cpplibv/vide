option(VIDE_BUILD_PORTABILITY_TEST "Build the 32 bit test variants and the 32/64 bit portability test (needs -m32 support)" OFF)

find_package(Threads REQUIRED)
find_package(Boost CONFIG QUIET)

include(${PROJECT_SOURCE_DIR}/cmake/CPM.cmake)

# Fetched only as top level project, a parent project's Boost setup is left alone
if (NOT Boost_FOUND AND PROJECT_IS_TOP_LEVEL)
	CPMAddPackage(
			NAME Boost
			VERSION 1.92.0
			# Using the b2 archive on purpose: one merged header tree, Smaller than the CMake one (nothing to compile)
			URL https://github.com/boostorg/boost/releases/download/boost-1.92.0/boost-1.92.0-b2-nodocs.tar.xz
			URL_HASH SHA256=ea7b982002cc9dfbe59b0b217b206f470dc75f3de0bb2973d844118934d82411
			DOWNLOAD_ONLY YES)
	if (Boost_ADDED)
		add_library(Boost::headers INTERFACE IMPORTED)
		target_include_directories(Boost::headers INTERFACE ${Boost_SOURCE_DIR})
	endif ()
endif ()

CPMAddPackage(
		NAME doctest
		GITHUB_REPOSITORY doctest/doctest
		VERSION 2.5.3
		SYSTEM YES
		OPTIONS "DOCTEST_NO_INSTALL ON" "DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF")

# Shared test code: doctest, the archive test case macros and helpers under common/. Header only, except the
# doctest main and the polymorphic class registrations, which are compiled once in separate libraries
add_library(vide_test_common INTERFACE)
file(GLOB vide_test_common_headers CONFIGURE_DEPENDS common/*.hpp polymorphic/*.hpp)
target_sources(vide_test_common INTERFACE FILE_SET HEADERS FILES ${vide_test_common_headers})
target_include_directories(vide_test_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vide_test_common INTERFACE vide::vide doctest::doctest Threads::Threads)
set_target_properties(vide_test_common PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON)

# vide_add_common_libraries(<suffix>): the compiled shared test code, suffix "_32" for the -m32 variants
function(vide_add_common_libraries suffix)
	add_library(vide_doctest_main${suffix} STATIC common/doctest_main.cpp)
	target_link_libraries(vide_doctest_main${suffix} PRIVATE vide_test_common)

	# OBJECT so the linker cannot drop the registrations
	add_library(vide_polymorphic_test_classes${suffix} OBJECT polymorphic/test_classes.cpp)
	target_link_libraries(vide_polymorphic_test_classes${suffix} PRIVATE vide_test_common)

	foreach (target IN ITEMS vide_doctest_main${suffix} vide_polymorphic_test_classes${suffix})
		if (MINGW)
			target_compile_options(${target} PRIVATE -Wa,--mbig-obj)
		endif ()
		if (suffix STREQUAL "_32")
			target_compile_options(${target} PRIVATE -m32)
		endif ()
	endforeach ()
endfunction()

vide_add_common_libraries("")
if (VIDE_BUILD_PORTABILITY_TEST)
	vide_add_common_libraries("_32")
endif ()

# vide_add_test(<source> [libraries...]): executable and test named after folder and file, the folder only once if
# the file already carries it: "core/pod.cpp" -> "test_core_pod", "std/std_vector.cpp" -> "test_std_vector"
function(vide_add_test source)
	cmake_path(GET source STEM stem)
	cmake_path(GET source PARENT_PATH folder)
	cmake_path(GET folder FILENAME folder)
	if (stem MATCHES "^${folder}_")
		set(name test_${stem})
	else ()
		set(name test_${folder}_${stem})
	endif ()

	set(variants ${name})
	if (VIDE_BUILD_PORTABILITY_TEST)
		list(APPEND variants ${name}_32)
	endif ()

	foreach (target IN LISTS variants)
		if (target MATCHES "_32$")
			set(suffix _32)
		else ()
			set(suffix "")
		endif ()

		add_executable(${target} ${source})
		target_link_libraries(${target} PRIVATE vide_test_common vide_doctest_main${suffix} ${ARGN})
		# Registrations of polymorphic/test_classes.hpp; linked into the whole folder, harmless where unused
		if (folder STREQUAL "polymorphic")
			target_link_libraries(${target} PRIVATE vide_polymorphic_test_classes${suffix})
		endif ()
		if (MINGW)
			# Large tests (like polymorphic) with debug info exceed the 16 bit COFF section index limit of GNU as
			target_compile_options(${target} PRIVATE -Wa,--mbig-obj)
		endif ()
		if (suffix STREQUAL "_32")
			target_compile_options(${target} PRIVATE -m32)
			target_link_options(${target} PRIVATE -m32)
		endif ()
		add_test(NAME ${target} COMMAND ${target})
	endforeach ()
endfunction()

foreach (dir IN ITEMS archives core memory polymorphic std)
	file(GLOB tests CONFIGURE_DEPENDS ${dir}/*.cpp)
	list(REMOVE_ITEM tests ${CMAKE_CURRENT_SOURCE_DIR}/polymorphic/test_classes.cpp)
	foreach (test IN LISTS tests)
		vide_add_test(${test})
	endforeach ()
endforeach ()

# Not Boost_FOUND: the CPM fallback leaves it false from the failed find_package; the target exists either way
if (TARGET Boost::headers)
	message(STATUS "Found boost: Adding boost related test targets")
	file(GLOB tests CONFIGURE_DEPENDS boost/*.cpp)
	foreach (test IN LISTS tests)
		vide_add_test(${test} Boost::headers)
	endforeach ()
endif ()

add_subdirectory(compile_fail)

if (VIDE_BUILD_PORTABILITY_TEST)
	add_executable(portability_test32 portability/portability.cpp)
	target_link_libraries(portability_test32 PRIVATE vide::vide)
	target_compile_options(portability_test32 PRIVATE -m32)
	target_link_options(portability_test32 PRIVATE -m32)

	add_executable(portability_test64 portability/portability.cpp)
	target_link_libraries(portability_test64 PRIVATE vide::vide)

	add_test(NAME portability_test COMMAND ${CMAKE_COMMAND}
			-DPORTABILITY_TEST_32=$<TARGET_FILE:portability_test32>
			-DPORTABILITY_TEST_64=$<TARGET_FILE:portability_test64>
			-P ${CMAKE_CURRENT_SOURCE_DIR}/portability/run_portability_test.cmake)
endif ()

if (VIDE_INSTALL)
	add_test(NAME test_cmake_config_module COMMAND ${CMAKE_COMMAND}
			-DVIDE_SOURCE_DIR=${PROJECT_SOURCE_DIR}
			-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/cmake_config_module
			-DCMAKE_GENERATOR=${CMAKE_GENERATOR}
			-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
			-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake-config-module.cmake)
endif ()
