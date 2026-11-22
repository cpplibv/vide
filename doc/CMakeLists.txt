find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
	return()
endif ()

configure_file(doxygen.in doxygen.cfg @ONLY)
add_custom_target(doc
		COMMAND Doxygen::doxygen ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM)

configure_file(${PROJECT_SOURCE_DIR}/scripts/updatedoc.in updatedoc.sh @ONLY)
add_custom_target(update-doc
		COMMAND ${CMAKE_CURRENT_BINARY_DIR}/updatedoc.sh
		DEPENDS doc
		COMMENT "Copying documentation to gh-pages branch"
		VERBATIM)
