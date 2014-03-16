cmake_minimum_required(VERSION 2.8)
project(ftracelib2-test)

set(CMAKE_BUILD_DIRECTORY Build/)
set(EXECUTABLE_OUTPUT_PATH Build/bin)

include_directories(
	src
)

FILE(GLOB sources src/*.cpp)

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} cppunit ftracelib2)

enable_testing()
add_test(tests Build/bin/${PROJECT_NAME})
