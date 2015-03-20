cmake_minimum_required(VERSION 2.8)
project(ftrace)


#Set build path relative to each project
set(CMAKE_BUILD_DIRECTORY Build)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Build/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Build/bin)
set(RESOURCES_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Build/resources)

# Root path
set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})


#Set default compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CTEST_LAUNCH_COMPILE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	#Compile with debug flags
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	#Compile withe optimisations on, -O3 mode is not recommended
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



#Create build package
#set(CPACK_GENERATOR "DEB")
#set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sebastien BESOMBES")
#set(CPACK_PACKAGE_VERSION "2.0.0")
#set(CPACK_PACKAGE_FILE_NAME  "lib${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
#include(CPack)

#install (   FILES Build/lib/libftracelib.so 
#            DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

add_subdirectory(ftracelib)
#Build Tests
add_subdirectory(ftracelib/tests/tu)
add_subdirectory(ftracelib/tests/tv)
add_subdirectory(ftracelib/tests/perfo)

#Tests
include( CTest )
enable_testing()