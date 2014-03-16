cmake_minimum_required(VERSION 2.8)
project(ftracelib2-tv)

set(CMAKE_BUILD_DIRECTORY Build/)
set(EXECUTABLE_OUTPUT_PATH Build/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -finstrument-functions")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -finstrument-functions")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

include_directories(
	src
)

FILE(GLOB sources src/*.cpp)

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} cppunit)

enable_testing()
add_test(tests runtest.sh)
