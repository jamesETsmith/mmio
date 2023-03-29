add_executable(mmio_read mmio_read.c)
target_include_directories(mmio_read PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(mmio_read PRIVATE mmio)