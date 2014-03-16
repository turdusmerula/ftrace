cmake_minimum_required(VERSION 2.8)
project(libftrace-test)

set(CMAKE_BUILD_DIRECTORY Build/)
set(EXECUTABLE_OUTPUT_PATH Build/bin)

#message("Build type ${CMAKE_BUILD_TYPE}")

find_package(Boost 1.53 COMPONENTS date_time REQUIRED)

include_directories(
	src
)

FILE(GLOB sources src/*.cpp)

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} cppunit ${Boost_LIBRARIES} )

enable_testing()
add_test(tests Build/bin/${PROJECT_NAME})
