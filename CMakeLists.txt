cmake_minimum_required(VERSION 3.1.3)
project(Quiver CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # otherwise, CMake will pass -std=gnu++17 to clang

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

include_directories("./include/")
include_directories("./ext/Catch2/single_include/catch2/")

add_executable(Quiver src/quiver.cpp)
find_package(Threads REQUIRED)
target_link_libraries(Quiver ${CMAKE_THREAD_LIBS_INIT})

file(GLOB_RECURSE Quiver_Test test/*.cpp)
add_executable(Test ${Quiver_Test})
