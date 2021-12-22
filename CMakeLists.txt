cmake_minimum_required(VERSION 3.20)

project(cereal LANGUAGES CXX VERSION 1.3.0)

if (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CEREAL_MASTER_PROJECT ON)
endif ()


include(ExternalProject)
include(cmake/wish_create.cmake)
include(cmake/wish_date.cmake)
include(cmake/wish_git.cmake)
include(cmake/wish_util.cmake)
include(cmake/wish_warning.cmake)


#if (APPLE)
#    option(SKIP_PORTABILITY_TEST "Skip portability (32 bit) tests" ON)
#endif ()
#
#option(SKIP_PERFORMANCE_COMPARISON "Skip building performance comparison (requires boost)" OFF)

# TODO: should not be needed! CK
if (NOT CMAKE_VERSION VERSION_LESS 3.0) # installing cereal requires INTERFACE lib
    option(JUST_INSTALL_CEREAL "Don't do anything besides installing the library" OFF)
endif ()


set(CEREAL_THREAD_LIBS)
if (UNIX)
    option(THREAD_SAFE "Use mutexes to ensure thread safety" OFF)
    if (THREAD_SAFE)
        message(STATUS "Use mutexes")
        add_definitions(-DCEREAL_THREAD_SAFE=1)
        set(CEREAL_THREAD_LIBS pthread)
    endif ()
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
    add_compile_options(-Og)
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
    add_compile_options(-static) # Normally wish_static_link_std() would take care of this, but cereal is not really using wish

elseif (CMAKE_BUILD_TYPE STREQUAL "DEV")
    add_compile_options(-O3)

else ()
    message(WARNING "Failed to identify [${CMAKE_BUILD_TYPE}] as a build type")
endif ()

add_compile_options(-m64)
add_compile_options(-std=c++23)


# -------------------------------------------------------------------------------------------------

add_library(cereal INTERFACE)
add_library(cereal::cereal ALIAS cereal)
target_include_directories(cereal INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )
list(APPEND CEREAL_THREAD_LIBS cereal::cereal)

if (NOT CMAKE_VERSION VERSION_LESS 3.8)
    target_compile_features(cereal INTERFACE cxx_std_11)
endif ()


option(CEREAL_INSTALL "Generate the install target" ${CEREAL_MASTER_PROJECT})
if (CEREAL_INSTALL)
    include(CMakePackageConfigHelpers)

    install(TARGETS cereal EXPORT ${PROJECT_NAME}Targets)
    install(DIRECTORY include/cereal DESTINATION include)

    set(configFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake)
    set(versionFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)
    set(configInstallDestination lib/cmake/${PROJECT_NAME})

    configure_package_config_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
            ${configFile}
            INSTALL_DESTINATION ${configInstallDestination}
    )
    write_basic_package_version_file(
            ${versionFile}
            COMPATIBILITY SameMajorVersion
    )

    install(FILES ${configFile} ${versionFile} DESTINATION ${configInstallDestination})
    install(
            EXPORT ${PROJECT_NAME}Targets
            NAMESPACE "cereal::"
            DESTINATION ${configInstallDestination}
    )
endif ()


if (JUST_INSTALL_CEREAL)
    return()
endif ()


#if(NOT SKIP_PERFORMANCE_COMPARISON)
#    # Boost serialization for performance sandbox
#    find_package(Boost REQUIRED COMPONENTS serialization)
#endif()


option(BUILD_TESTS "Build tests" ${CEREAL_MASTER_PROJECT})
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(unittests)
endif ()

add_subdirectory(sandbox)

add_subdirectory(doc)
