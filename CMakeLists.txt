cmake_minimum_required(VERSION 2.8)
project(ftrace2)

#Build Tests
add_subdirectory(ftracelib2)
add_subdirectory(ftrace)

#Create build package
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sebastien BESOMBES")
set(CPACK_PACKAGE_VERSION "2.0.0")
set(CPACK_PACKAGE_FILE_NAME  "lib${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)

install (   FILES ftracelib2/Build/lib/libftracelib2.so 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install (   FILES ftrace/Build/bin/ftracebin    
PERMISSIONS GROUP_EXECUTE GROUP_READ OWNER_EXECUTE OWNER_READ WORLD_EXECUTE WORLD_READ 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/ RENAME ftrace)
#            FILE_PERMISSIONS "555")

#Tests
enable_testing()
