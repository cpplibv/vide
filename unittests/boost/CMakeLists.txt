find_package(Boost CONFIG QUIET)
if (NOT Boost_FOUND)
	return()
endif ()

message(STATUS "Found boost: Adding boost related test targets")

file(GLOB tests CONFIGURE_DEPENDS *.cpp)
foreach (test IN LISTS tests)
	vide_add_test(${test} Boost::headers)
endforeach ()
