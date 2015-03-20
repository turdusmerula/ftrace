project(ftracelib)

find_package(Boost 1.53 COMPONENTS 
	filesystem regex system 
	REQUIRED)

include_directories(
	${Boost_INCLUDE_DIR}
	src
	../gcc-demangle
	)

LINK_DIRECTORIES(
	${Boost_LIBRARY_DIRS}
	)

FILE(GLOB sources 
	src/*.cpp 
	src/com/*.cpp 
	src/err/*.cpp 
	src/log/*.cpp 
	src/mem/*.cpp 
	src/objects/*.cpp 
	src/scope/*.cpp
	../gcc-demangle/cp-demangle.c
	)

FILE(GLOB headers 
	src/*.h
	src/com/*.h 
	src/err/*.h 
	src/log/*.h 
	src/mem/*.h 
	src/objects/*.h 
	src/scope/*.h
	../gcc-demangle/*.h
	)

add_library(${PROJECT_NAME} SHARED ${sources})
target_link_libraries(${PROJECT_NAME} 
	${Boost_LIBRARIES} 
	iberty
)
