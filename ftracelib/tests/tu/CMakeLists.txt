project(libftrace-test)

find_package(Boost 1.53 COMPONENTS 
	date_time 
	REQUIRED)

include_directories(
	src
	)

FILE(GLOB sources 
	src/*.cpp
	)

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} 
	cppunit 
	${Boost_LIBRARIES} 
	)
