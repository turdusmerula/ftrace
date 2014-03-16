#Those tests are disabled by default

cmake_minimum_required(VERSION 2.8)
project(ftracebin-perfo)

set(CMAKE_BUILD_DIRECTORY Build/)
set(EXECUTABLE_OUTPUT_PATH Build/bin)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


find_package(Boost 1.53 COMPONENTS date_time REQUIRED)

include_directories(
	src
)

FILE(GLOB sources src/*.cpp)

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} cppunit ${Boost_LIBRARIES} )
