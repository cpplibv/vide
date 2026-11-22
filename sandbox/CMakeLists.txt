option(VIDE_BUILD_PERFORMANCE_COMPARISON "Build the performance comparison against Boost.Serialization" OFF)

add_subdirectory(sandbox_shared_lib)

foreach (name IN ITEMS sandbox sandbox_json sandbox_as sandbox_rtti)
	add_executable(${name} ${name}.cpp)
	target_link_libraries(${name} PRIVATE vide::vide)
endforeach ()

add_executable(sandbox_vs sandbox_vs.cpp)
target_link_libraries(sandbox_vs PRIVATE sandbox_vs_dll)
if (NOT WIN32)
	# Drops the Release -static: a static executable cannot load a shared library (MinGW links the DLL through its import library)
	set_property(TARGET sandbox_vs PROPERTY LINK_OPTIONS "")
endif ()

if (VIDE_BUILD_PERFORMANCE_COMPARISON)
	find_package(Boost CONFIG REQUIRED COMPONENTS serialization)
	add_executable(performance performance.cpp)
	target_link_libraries(performance PRIVATE vide::vide Boost::serialization)
endif ()
