cmake_minimum_required(VERSION 3.12)
project(Quiver CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # otherwise, CMake will pass -std=gnu++20 to clang

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)
if(NOT COMMAND FetchContent_MakeAvailable)
	macro(FetchContent_MakeAvailable NAME)
		FetchContent_GetProperties(${NAME})
		string(TOLOWER "${NAME}" lcName)
		if(NOT ${lcName}_POPULATED)
			FetchContent_Populate(${NAME})
			add_subdirectory("${${lcName}_SOURCE_DIR}" "${${lcName}_BINARY_DIR}" EXCLUDE_FROM_ALL)
		endif()
	endmacro()
endif()

option(QUIVER_BUILD_TESTS "Build Quiver's unit tests" OFF)
option(QUIVER_BUILD_FRONTEND "Build Quiver's frontend" OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
   CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
   CMAKE_CXX_COMPILER_ID MATCHES "Intel")
	set(QUIVER_GNU_OPTIONS TRUE)
endif()

if(QUIVER_GNU_OPTIONS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-deprecated -fno-rtti")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb -fno-omit-frame-pointer")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -DNDEBUG -s -flto")
endif()

add_library(Quiver INTERFACE)
target_include_directories(Quiver INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include/")

add_executable(QuiverFrontend "${CMAKE_CURRENT_LIST_DIR}/src/quiver.cpp")
find_package(Threads REQUIRED)
target_link_libraries(QuiverFrontend PRIVATE
	${CMAKE_THREAD_LIBS_INIT}
	Quiver
)

if(QUIVER_BUILD_TESTS)
	FetchContent_Declare(
		Catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG        5c88067bd339465513af4aec606bd2292f1b594a
		# GIT_TAG        v2.13.6
	)
	FetchContent_MakeAvailable(Catch2)

	file(GLOB_RECURSE QuiverTestSrcs "${CMAKE_CURRENT_LIST_DIR}/test/*.cpp")
	add_executable(QuiverTest ${QuiverTestSrcs})
	target_link_libraries(QuiverTest PRIVATE
		Quiver
		Catch2::Catch2
	)
endif()
