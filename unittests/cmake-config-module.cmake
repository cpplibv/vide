# Installs vide into WORK_DIR, then builds and runs a consumer through find_package(vide)
# Usage: cmake -DVIDE_SOURCE_DIR=<dir> -DWORK_DIR=<dir> -DCMAKE_GENERATOR=<gen> -DCMAKE_CXX_COMPILER=<cxx> -P cmake-config-module.cmake

set(install_dir ${WORK_DIR}/install)
set(consumer_source_dir ${WORK_DIR}/consumer_source)
set(generator_and_compiler -G ${CMAKE_GENERATOR} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})

file(REMOVE_RECURSE ${WORK_DIR})

function(run_checked)
	execute_process(COMMAND ${ARGN} COMMAND_ERROR_IS_FATAL ANY)
endfunction()

run_checked(${CMAKE_COMMAND} -S ${VIDE_SOURCE_DIR} -B ${WORK_DIR}/vide ${generator_and_compiler}
		-DVIDE_JUST_INSTALL=ON
		-DCMAKE_INSTALL_PREFIX=${install_dir})
run_checked(${CMAKE_COMMAND} --install ${WORK_DIR}/vide)

file(WRITE ${consumer_source_dir}/CMakeLists.txt [=[
cmake_minimum_required(VERSION 3.24)
project(vide_config_module_consumer LANGUAGES CXX)
find_package(vide 3 REQUIRED)
add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE vide::vide)
]=])

file(WRITE ${consumer_source_dir}/main.cpp [=[
#include <vide/archives/binary.hpp>
#include <vide/archives/json.hpp>
#include <vide/archives/xml.hpp>
#include <cstdlib>
#include <sstream>

struct MyData {
	int x = 0, y = 0, z = 0;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(x);
		ar(y);
		ar(z);
	}
};

int main() {
	std::stringstream ss;
	{
		vide::BinaryOutputArchive ar(ss);
		ar(MyData{1, 2, 3});
	}
	MyData data;
	{
		vide::BinaryInputArchive ar(ss);
		ar(data);
	}
	return data.x == 1 && data.y == 2 && data.z == 3 ? EXIT_SUCCESS : EXIT_FAILURE;
}
]=])

run_checked(${CMAKE_COMMAND} -S ${consumer_source_dir} -B ${WORK_DIR}/consumer ${generator_and_compiler}
		-DCMAKE_PREFIX_PATH=${install_dir})
run_checked(${CMAKE_COMMAND} --build ${WORK_DIR}/consumer)
run_checked(${WORK_DIR}/consumer/consumer)
