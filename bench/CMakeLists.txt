add_executable(mmio_read mmio_read.c)
target_include_directories(mmio_read PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(mmio_read PRIVATE mmio)

add_executable(lagr_mmio_read lagr_mmio_read.c)
target_include_directories(
    lagr_mmio_read PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    /home/james/apps/GraphBLAS/build/install/include/
    /home/james/apps/LAGraph/build/install/include/
    )
target_link_libraries(
    lagr_mmio_read PUBLIC 
    /home/james/apps/LAGraph/build/install/lib/liblagraph.a
    /home/james/apps/GraphBLAS/build/install/lib/libgraphblas.a
    m
    OpenMP::OpenMP_C
    )