cmake_minimum_required(VERSION 3.16)

project(mmio)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "Export compile commands to build directory" FORCE)

set(COMMON_FLAGS "-Wall -O3 -march=native -fPIC")
set(CMAKE_C_FLAGS "-std=c11 ${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")

find_package(OpenMP REQUIRED)

add_library(mmio src/mmio.c)
set_property(TARGET mmio PROPERTY C_STANDARD 11)
target_include_directories(mmio PRIVATE include)

add_library(mtx_io src/mtx_io.c)
set_property(TARGET mtx_io PROPERTY C_STANDARD 11)
target_include_directories(mtx_io PRIVATE include)
target_link_libraries(mtx_io OpenMP::OpenMP_C)


# Get and install fast_matrix_market
include(FetchContent)
FetchContent_Declare(
        fast_matrix_market
        GIT_REPOSITORY https://github.com/alugowski/fast_matrix_market
        GIT_TAG main
        GIT_SHALLOW TRUE
)

FetchContent_Declare(
        PIGO
        GIT_REPOSITORY https://github.com/GT-TDAlab/PIGO
        GIT_TAG master
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fast_matrix_market PIGO)

add_subdirectory(bench)