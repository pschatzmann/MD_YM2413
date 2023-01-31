cmake_minimum_required(VERSION 3.20)

# set the project name
project(ym2413-dsk)
set (CMAKE_CXX_STANDARD 11)
set (DCMAKE_CXX_FLAGS "-Werror")

include(FetchContent)

# Build with ym2413 library
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/ym2413 )
endif()

# build sketch as executable
add_executable (ym2413-dsk main.cpp)

# specify libraries
target_link_libraries(ym2413-dsk ym2413)
