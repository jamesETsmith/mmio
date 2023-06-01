add_executable(mmio_read mmio_read.c)
target_include_directories(mmio_read PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(mmio_read PRIVATE mmio)

add_executable(io io.cpp)
target_link_libraries(io OpenMP::OpenMP_CXX)

add_executable(io_c io_c.c)
target_link_libraries(io_c OpenMP::OpenMP_C)

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

# Get and install fast_matrix_market
include(FetchContent)
FetchContent_Declare(
        fast_matrix_market
        GIT_REPOSITORY https://github.com/alugowski/fast_matrix_market
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fast_matrix_market)


add_executable(fmm_read fmm_read.cpp)
target_include_directories(
    fmm_read PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    /home/james/apps/GraphBLAS/build/install/include/
    )
target_link_libraries(fmm_read fast_matrix_market::fast_matrix_market
    /home/james/apps/GraphBLAS/build/install/lib/libgraphblas.a
    m
    OpenMP::OpenMP_CXX
)