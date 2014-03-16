cmake_minimum_required(VERSION 2.8)
project(ftracelib2)

set(CMAKE_BUILD_DIRECTORY Build/)
set(LIBRARY_OUTPUT_PATH Build/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CTEST_LAUNCH_COMPILE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "gcov")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
	set(COVERAGE_COMMAND "${COVERAGE_COMMAND} /usr/bin/gcov")
endif()

if(PROFILE_TYPE STREQUAL "valgrind")
	set(CTEST_MEMORYCHECK_COMMAND "/usr/bin/valgrind")
	set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --quiet --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=100 --verbose --demangle=yes")
endif()


message("Build type ${CMAKE_BUILD_TYPE}")
message("Profile type ${PROFILE_TYPE}")

include_directories(
	src
)

FILE(GLOB sources src/*.cpp src/com/*.cpp src/err/*.cpp src/log/*.cpp src/mem/*.cpp src/objects/*.cpp src/scope/*.cpp)
FILE(GLOB headers src/*.h)

add_library(${PROJECT_NAME} SHARED ${sources})
target_link_libraries(${PROJECT_NAME} iberty_pic pthread rt)


#Build Tests
add_subdirectory(tests/tu)
add_subdirectory(tests/tv)
add_subdirectory(tests/perfo)

#Tests
include( CTest )
enable_testing()
