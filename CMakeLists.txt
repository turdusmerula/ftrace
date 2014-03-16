cmake_minimum_required(VERSION 2.8)
project(ftrace)

#Build Tests
add_subdirectory(ftracelib)

#Create build package
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sebastien BESOMBES")
set(CPACK_PACKAGE_VERSION "2.0.0")
set(CPACK_PACKAGE_FILE_NAME  "lib${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)

install (   FILES ftracelib/Build/lib/libftracelib.so 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

#Tests
enable_testing()
