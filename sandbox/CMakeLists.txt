add_subdirectory(sandbox_shared_lib)

add_executable(sandbox sandbox.cpp)
target_link_libraries(sandbox ${VIDE_THREAD_LIBS})

add_executable(sandbox_json sandbox_json.cpp)
target_link_libraries(sandbox_json ${VIDE_THREAD_LIBS})

add_executable(sandbox_as sandbox_as.cpp)
target_link_libraries(sandbox_as ${VIDE_THREAD_LIBS})

add_executable(sandbox_rtti sandbox_rtti.cpp)
target_link_libraries(sandbox_rtti ${VIDE_THREAD_LIBS})

add_executable(sandbox_vs sandbox_vs.cpp)
target_link_libraries(sandbox_vs sandbox_vs_dll)

if (Boost_FOUND AND NOT SKIP_PERFORMANCE_COMPARISON)
    add_executable(performance performance.cpp)
    if (MSVC)
        set_target_properties(performance PROPERTIES COMPILE_DEFINITIONS "BOOST_SERIALIZATION_DYN_LINK")
    endif ()
    target_include_directories(performance PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(performance ${VIDE_THREAD_LIBS} ${Boost_LIBRARIES})
endif ()
