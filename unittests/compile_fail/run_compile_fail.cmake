# Builds one compile-fail target, then checks the build output against the expectation lines of its source:
#   // Expected: <regex>        matched with the ANSI color escapes removed, so passes with colors on or off
#   // Expected raw: <regex>    matched against the output as is, escapes included
# Every line must match, the build must fail, and with a single error: follow-up errors bury the message
# Usage: cmake -DSOURCE=<file.cpp> -DTARGET=<target> -DBINARY_DIR=<build tree> -DCONFIG=<config> -P run_compile_fail.cmake

# Ninja strips color escapes from output that is not a terminal, the raw expectations need them kept
set(ENV{CLICOLOR_FORCE} 1)
execute_process(
		COMMAND ${CMAKE_COMMAND} --build ${BINARY_DIR} --target ${TARGET} --config ${CONFIG}
		OUTPUT_VARIABLE raw_output
		ERROR_VARIABLE raw_output
		RESULT_VARIABLE build_result)

string(ASCII 27 escape)
# "\033[01;31m\033[K" -> "": the compiler's own coloring (-fdiagnostics-color) as well as VIDE_DIAGNOSTIC_COLORS
string(REGEX REPLACE "${escape}\\[[0-9;]*[A-Za-z]" "" output "${raw_output}")

set(failures "")
if (build_result EQUAL 0)
	string(APPEND failures "\n  the build succeeded")
endif ()

string(REGEX MATCHALL "error:" errors "${output}")
list(LENGTH errors error_count)
if (error_count GREATER 1)
	string(APPEND failures "\n  ${error_count} errors instead of one")
endif ()

# Parsed by hand, not through file(STRINGS): CMake lists would split or join regexes on ';' and unbalanced '['
file(READ ${SOURCE} source)
string(PREPEND source "\n")
set(expected_count 0)
while (TRUE)
	string(FIND "${source}" "\n// Expected" line_start)
	if (line_start EQUAL -1)
		break ()
	endif ()
	math(EXPR line_start "${line_start} + 1")
	string(SUBSTRING "${source}" ${line_start} -1 source)
	string(FIND "${source}" "\n" line_end)
	if (line_end EQUAL -1)
		set(line "${source}")
		set(source "")
	else ()
		string(SUBSTRING "${source}" 0 ${line_end} line)
		string(SUBSTRING "${source}" ${line_end} -1 source)
	endif ()
	string(REGEX REPLACE "\r$" "" line "${line}")

	if (line MATCHES "^// Expected: (.*)$")
		set(regex "${CMAKE_MATCH_1}")
		set(subject "${output}")
	elseif (line MATCHES "^// Expected raw: (.*)$")
		set(regex "${CMAKE_MATCH_1}")
		set(subject "${raw_output}")
	else ()
		string(APPEND failures "\n  unknown expectation line: ${line}")
		continue ()
	endif ()

	math(EXPR expected_count "${expected_count} + 1")
	string(REGEX MATCH "${regex}" match "${subject}")
	if (match STREQUAL "")
		string(APPEND failures "\n  no match for: ${line}")
	endif ()
endwhile ()

if (expected_count EQUAL 0)
	string(APPEND failures "\n  ${SOURCE} has no \"// Expected: <regex>\" line")
endif ()

if (NOT failures STREQUAL "")
	message("${raw_output}")
	message(FATAL_ERROR "Compile-fail test ${TARGET} failed:${failures}")
endif ()
