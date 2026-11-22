option(VIDE_BUILD_PORTABILITY_TEST "Build the 32 bit test variants and the 32/64 bit portability test (needs -m32 support)" OFF)

find_package(Threads REQUIRED)

# doctest's runner and main
function(vide_add_doctest_main suffix)
	add_library(vide_doctest_main${suffix} STATIC common/doctest_main.cpp)
	target_compile_features(vide_doctest_main${suffix} PRIVATE cxx_std_26)
	if (suffix STREQUAL "_32")
		target_compile_options(vide_doctest_main${suffix} PRIVATE -m32)
	endif ()
endfunction()

vide_add_doctest_main("")
if (VIDE_BUILD_PORTABILITY_TEST)
	vide_add_doctest_main("_32")
endif ()

# vide_add_test(<source> [libraries...]): "pod.cpp" -> executable and test "test_pod"
function(vide_add_test source)
	cmake_path(GET source STEM name)
	set(variants test_${name})
	if (VIDE_BUILD_PORTABILITY_TEST)
		list(APPEND variants test_${name}_32)
	endif ()

	foreach (target IN LISTS variants)
		add_executable(${target} ${source})
		target_link_libraries(${target} PRIVATE vide::vide Threads::Threads ${ARGN})
		if (MINGW)
			# Large tests (like polymorphic) with debug info exceed the 16 bit COFF section index limit of GNU as
			target_compile_options(${target} PRIVATE -Wa,--mbig-obj)
		endif ()
		if (target MATCHES "_32$")
			target_compile_options(${target} PRIVATE -m32)
			target_link_options(${target} PRIVATE -m32)
			set(suffix _32)
		else ()
			set(suffix "")
		endif ()
		target_link_libraries(${target} PRIVATE vide_doctest_main${suffix})
		add_test(NAME ${target} COMMAND ${target})
	endforeach ()
endfunction()

file(GLOB tests CONFIGURE_DEPENDS *.cpp)
list(REMOVE_ITEM tests ${CMAKE_CURRENT_SOURCE_DIR}/portability_test.cpp)
foreach (test IN LISTS tests)
	vide_add_test(${test})
endforeach ()

if (VIDE_BUILD_PORTABILITY_TEST)
	add_executable(portability_test32 portability_test.cpp)
	target_link_libraries(portability_test32 PRIVATE vide::vide)
	target_compile_options(portability_test32 PRIVATE -m32)
	target_link_options(portability_test32 PRIVATE -m32)

	add_executable(portability_test64 portability_test.cpp)
	target_link_libraries(portability_test64 PRIVATE vide::vide)

	add_test(NAME portability_test COMMAND ${CMAKE_COMMAND}
			-DPORTABILITY_TEST_32=$<TARGET_FILE:portability_test32>
			-DPORTABILITY_TEST_64=$<TARGET_FILE:portability_test64>
			-P ${CMAKE_CURRENT_SOURCE_DIR}/run_portability_test.cmake)
endif ()

if (VIDE_INSTALL)
	add_test(NAME test_cmake_config_module COMMAND ${CMAKE_COMMAND}
			-DVIDE_SOURCE_DIR=${PROJECT_SOURCE_DIR}
			-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/cmake_config_module
			-DCMAKE_GENERATOR=${CMAKE_GENERATOR}
			-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
			-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake-config-module.cmake)
endif ()

add_subdirectory(boost)
