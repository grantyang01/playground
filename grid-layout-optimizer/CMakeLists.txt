cmake_minimum_required(VERSION 3.20)
project(grid-layout-optimizer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(grid-layout-optimizer
    src/main.cpp
)

target_include_directories(grid-layout-optimizer PRIVATE src)

if(MSVC)
    target_compile_options(grid-layout-optimizer PRIVATE /W4 /O2)
else()
    target_compile_options(grid-layout-optimizer PRIVATE -Wall -Wextra -O3)
endif()
