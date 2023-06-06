cmake_minimum_required(VERSION 3.16)

project(mmio)

set(CMAKE_EXPORT_COMPILE_COMMANDS
    TRUE
    CACHE BOOL "Export compile commands to build directory" FORCE)

set(COMMON_FLAGS "-Wall -O3 -march=native -fPIC")
set(CMAKE_C_FLAGS "-std=c11 ${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")

find_package(OpenMP REQUIRED)

add_library(mmio src/mmio.c)
set_property(TARGET mmio PROPERTY C_STANDARD 11)
target_include_directories(mmio PUBLIC include)

add_library(mtxio src/mtxio.c)
set_property(TARGET mtxio PROPERTY C_STANDARD 11)
target_include_directories(mtxio PUBLIC include)
target_link_libraries(mtxio OpenMP::OpenMP_C)

# Get and install fast_matrix_market
include(FetchContent)
FetchContent_Declare(
  fast_matrix_market
  GIT_REPOSITORY https://github.com/alugowski/fast_matrix_market
  GIT_TAG main
  GIT_SHALLOW TRUE)

FetchContent_Declare(
  PIGO
  GIT_REPOSITORY https://github.com/GT-TDAlab/PIGO
  GIT_TAG master
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(fast_matrix_market PIGO)

add_subdirectory(bench)

option(MTXIO_TEST "Build tests for MTXIO")
if(${MTXIO_TEST})
  enable_testing()

  include(FetchContent)
  FetchContent_Declare(
    Tau
    GIT_REPOSITORY https://github.com/jasmcaus/tau
    GIT_TAG dev)
  FetchContent_MakeAvailable(Tau)

  add_subdirectory(test)

endif()
