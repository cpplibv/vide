# Compile-fail tests: each .cpp must fail to compile with the diagnostics its "// Expected: <regex>" lines describe,
# see run_compile_fail.cmake. A source that compiles fails the test too.
# "compile_fail/missing_save.cpp" -> test and target "test_compile_fail_missing_save"

file(GLOB sources CONFIGURE_DEPENDS *.cpp)
foreach (source IN LISTS sources)
	cmake_path(GET source STEM stem)
	set(name test_compile_fail_${stem})

	add_library(${name} OBJECT EXCLUDE_FROM_ALL ${source})
	target_link_libraries(${name} PRIVATE vide::vide)

	add_test(NAME ${name} COMMAND ${CMAKE_COMMAND}
			-DSOURCE=${source}
			-DTARGET=${name}
			-DBINARY_DIR=${CMAKE_BINARY_DIR}
			-DCONFIG=$<CONFIG>
			-P ${CMAKE_CURRENT_SOURCE_DIR}/run_compile_fail.cmake)
	# One nested build at a time: parallel builds in the same tree race on the build system's state files
	set_tests_properties(${name} PROPERTIES RESOURCE_LOCK vide_compile_fail_build)
endforeach ()
