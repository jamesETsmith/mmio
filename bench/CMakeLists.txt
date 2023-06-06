# My benchmarks
add_executable(mmio_read mmio_read.c)
target_include_directories(mmio_read PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(mmio_read PRIVATE mmio)

add_executable(io io.cpp)
target_link_libraries(io OpenMP::OpenMP_CXX)
# target_compile_options(io PRIVATE -fanalyzer)

add_executable(mtxio_read mtxio_read.c)
target_include_directories(mtxio_read PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(mtxio_read mtxio OpenMP::OpenMP_C)
target_compile_options(mtxio_read PRIVATE)

# External benchmarks
add_executable(lagr_mmio_read lagr_mmio_read.c)
target_include_directories(
  lagr_mmio_read
  PRIVATE ${CMAKE_SOURCE_DIR}/include
          /home/james/apps/GraphBLAS/build/install/include/
          /home/james/apps/LAGraph/build/install/include/)
target_link_libraries(
  lagr_mmio_read
  PUBLIC /home/james/apps/LAGraph/build/install/lib/liblagraph.a
         /home/james/apps/GraphBLAS/build/install/lib/libgraphblas.a m
         OpenMP::OpenMP_C)

add_executable(fmm_read fmm_read.cpp)
target_include_directories(
  fmm_read PRIVATE ${CMAKE_SOURCE_DIR}/include
                   /home/james/apps/GraphBLAS/build/install/include/)
target_link_libraries(
  fmm_read fast_matrix_market::fast_matrix_market
  /home/james/apps/GraphBLAS/build/install/lib/libgraphblas.a m
  OpenMP::OpenMP_CXX)

add_executable(pigo_read pigo_read.cpp)
target_include_directories(pigo_read PRIVATE ${pigo_SOURCE_DIR}/release)
target_link_libraries(pigo_read pigo OpenMP::OpenMP_CXX)
