cmake_minimum_required(VERSION 3.16)

project(mmio)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "Export compile commands to build directory" FORCE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -pg -Wall -O3")

add_library(mmio src/mmio.c)
set_property(TARGET mmio PROPERTY C_STANDARD 11)
target_include_directories(mmio PRIVATE include)

add_subdirectory(bench)