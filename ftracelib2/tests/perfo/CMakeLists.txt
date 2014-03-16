#Those tests are disabled by default

cmake_minimum_required(VERSION 2.8)
project(ftracelib2-perfo)

set(CMAKE_BUILD_DIRECTORY Build/)
set(EXECUTABLE_OUTPUT_PATH Build/bin)

include_directories(
	src
)

FILE(GLOB sources src/*.cpp)

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} cppunit)
