cmake_minimum_required(VERSION 3.1.3)
project(Quiver CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # otherwise, CMake will pass -std=gnu++17 to clang

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-missing-braces -fno-rtti -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb -fno-omit-frame-pointer -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -DNDEBUG")

include_directories("./include/")
include_directories("./ext/")

add_executable(Quiver src/quiver.cpp)
find_package(Threads REQUIRED)
target_link_libraries(Quiver ${CMAKE_THREAD_LIBS_INIT})

file(GLOB_RECURSE Quiver_Test test/*.cpp)
add_executable(Test ${Quiver_Test})
