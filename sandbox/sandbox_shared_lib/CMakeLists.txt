add_library(sandbox_vs_dll SHARED base.cpp derived.cpp)
target_link_libraries(sandbox_vs_dll PUBLIC vide::vide)
target_include_directories(sandbox_vs_dll PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
