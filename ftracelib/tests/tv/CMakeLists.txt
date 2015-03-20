project(libftrace-tv)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -finstrument-functions")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -finstrument-functions")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

find_package(Boost 1.53 COMPONENTS 
	date_time 
	REQUIRED)

include_directories(
	src
	)


FILE(GLOB sources 
	src/test-ldpreload.cpp
	)

add_executable(libftrace-ldpreload-tv ${sources})
target_link_libraries(libftrace-ldpreload-tv 
	cppunit 
	${Boost_LIBRARIES} 
	)
	

FILE(GLOB sources 
	src/test-linked.cpp
	)

add_executable(libftrace-linked-tv ${sources})
target_link_libraries(libftrace-linked-tv
	cppunit 
	${Boost_LIBRARIES}
	ftracelib
	)
	