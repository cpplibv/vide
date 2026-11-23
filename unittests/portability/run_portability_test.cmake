# Usage: cmake -DPORTABILITY_TEST_32=<exe> -DPORTABILITY_TEST_64=<exe> -P run_portability_test.cmake

function(run_checked)
	message(STATUS "Running ${ARGN}")
	execute_process(COMMAND ${ARGN} COMMAND_ERROR_IS_FATAL ANY)
endfunction()

run_checked(${PORTABILITY_TEST_64} save 64)
run_checked(${PORTABILITY_TEST_32} load 32)
run_checked(${PORTABILITY_TEST_32} save 32)
run_checked(${PORTABILITY_TEST_64} load 64)
run_checked(${PORTABILITY_TEST_64} remove 64)
