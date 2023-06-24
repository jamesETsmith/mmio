cmake_minimum_required(VERSION 3.16)

project(mmio)

include(cmake/CPM.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS
    TRUE
    CACHE BOOL "Export compile commands to build directory" FORCE)

set(COMMON_FLAGS "-Wall -O3 -march=native -fPIC")
set(CMAKE_C_FLAGS "-std=c11 ${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_EXPORT_COMPILE_COMMANDS
    TRUE
    CACHE BOOL "Export compile commands" FORCE)

find_package(OpenMP REQUIRED)

add_library(mmio src/mmio.c)
set_property(TARGET mmio PROPERTY C_STANDARD 11)
target_include_directories(mmio PUBLIC include)

add_library(mtxio src/mtxio.c)
set_property(TARGET mtxio PROPERTY C_STANDARD 11)
target_include_directories(mtxio PUBLIC include)
target_link_libraries(mtxio OpenMP::OpenMP_C m)

# Get and install fast_matrix_market
cpmaddpackage("gh:alugowski/fast_matrix_market@1.6.0")
cpmaddpackage("gh:GT-TDAlab/PIGO@0.6")

add_subdirectory(bench)

option(BUILD_TESTING "Build tests for MTXIO" ON)
if(${BUILD_TESTING})
  enable_testing()
  cpmaddpackage("gh:jasmcaus/tau#bde8709")

  add_subdirectory(test)

endif()
