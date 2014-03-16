cmake_minimum_required(VERSION 2.8)
project(ftracebin)

set(CMAKE_BUILD_DIRECTORY Build/)
set(EXECUTABLE_OUTPUT_PATH Build/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Boost 1.53 COMPONENTS filesystem regex system REQUIRED)

include_directories(
	${Boost_INCLUDE_DIR}
	src/
	../ftracelib2/src
)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

FILE(GLOB sources src/*.cpp)

add_executable(ftracebin ${sources})
target_link_libraries(ftracebin ${Boost_LIBRARIES} pthread rt)

#Build Tests
#add_subdirectory(tests/tu)
#add_subdirectory(tests/perfo)

#Tests
enable_testing()