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

add_library(Quiver INTERFACE)
target_include_directories(Quiver INTERFACE "./include/")

add_executable(QuiverFrontend src/quiver.cpp)
find_package(Threads REQUIRED)
target_link_libraries(QuiverFrontend PRIVATE
	${CMAKE_THREAD_LIBS_INIT}
	Quiver
)

file(GLOB_RECURSE QuiverTest test/*.cpp)
add_executable(Test ${QuiverTest})
target_include_directories(Test PRIVATE "./ext/Catch2/single_include/catch2/")
target_link_libraries(Test PRIVATE Quiver)
