# Compile-fail tests: each .cpp must fail to compile with a diagnostic matching its "// Expected: <regex>" line,
# and with that one error only: follow-up errors bury the message. A source that compiles fails the test too.
# "compile_fail/missing_save.cpp" -> test and target "test_compile_fail_missing_save"

file(GLOB sources CONFIGURE_DEPENDS *.cpp)
foreach (source IN LISTS sources)
	cmake_path(GET source STEM stem)
	set(name test_compile_fail_${stem})

	# The expectation lives in the source, so editing it has to reconfigure
	set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${source})
	file(STRINGS ${source} expected REGEX "^// Expected: " LIMIT_COUNT 1)
	if (NOT expected)
		message(FATAL_ERROR "${source}: missing the \"// Expected: <regex>\" line inside the unit test file")
	endif ()
	string(REGEX REPLACE "^// Expected: " "" expected "${expected}")

	add_library(${name} OBJECT EXCLUDE_FROM_ALL ${source})
	target_link_libraries(${name} PRIVATE vide::vide)

	add_test(NAME ${name} COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${name} --config $<CONFIG>)
	# The regexes alone decide (FAIL wins), the failing exit code is expected. WILL_FAIL would invert it.
	# One nested build at a time: parallel builds in the same tree race on the build system's state files
	set_tests_properties(${name} PROPERTIES
			PASS_REGULAR_EXPRESSION "${expected}"
			FAIL_REGULAR_EXPRESSION "error:.*error:"
			# Ninja strips color escapes from output that is not a terminal, diagnostic_colors.cpp needs them kept
			ENVIRONMENT CLICOLOR_FORCE=1
			RESOURCE_LOCK vide_compile_fail_build)
endforeach ()
