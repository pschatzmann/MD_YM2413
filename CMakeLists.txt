cmake_minimum_required(VERSION 3.16)

# set the project name
project(ym2413)

# lots of warnings and all warnings as errors
## add_compile_options(-Wall -Wextra )
set(CMAKE_CXX_STANDARD 17)

# define libraries
add_library (ym2413 
    "${PROJECT_SOURCE_DIR}/src/MD_YM2413.cpp"
    "${PROJECT_SOURCE_DIR}/src/MD_YM2413_hw.cpp"
    "${PROJECT_SOURCE_DIR}/src/emu2413/emu2413.c"
)

target_compile_options(ym2413 PRIVATE  -Wdouble-promotion -Wno-ignored-attributes)
#target_compile_definitions(ym2413 PUBLIC -DARDUINO)

# define location for header files
target_include_directories(ym2413 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
