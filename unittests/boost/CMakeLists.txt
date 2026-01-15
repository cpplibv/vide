find_package(Boost CONFIG QUIET)

if(Boost_FOUND)
    message(STATUS "Found boost: Adding boost related test targets")
else()
	return()
endif()


file(GLOB TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp)

# Build all of the non-special tests
foreach (TEST_SOURCE ${TESTS})

    string(REPLACE ".cpp" "" TEST_TARGET "${TEST_SOURCE}")
    set(TEST_TARGET "test_${TEST_TARGET}")

	add_executable(${TEST_TARGET} ${TEST_SOURCE})
	target_link_libraries(${TEST_TARGET} ${VIDE_THREAD_LIBS})
	add_test(NAME "${TEST_TARGET}" COMMAND "${TEST_TARGET}")

	# If we are on a 64-bit machine, create an extra 32-bit version of the test if portability testing is enabled
	if ((NOT MSVC) AND (${CMAKE_SIZEOF_VOID_P} EQUAL 8) AND (NOT SKIP_PORTABILITY_TEST))
		add_executable(${TEST_TARGET}_32 ${TEST_SOURCE})
		set_target_properties(${TEST_TARGET}_32 PROPERTIES
				COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
		target_link_libraries(${TEST_TARGET}_32 ${VIDE_THREAD_LIBS})
		add_test(NAME "${TEST_TARGET}_32" COMMAND "${TEST_TARGET}_32")
	endif ()

endforeach ()
