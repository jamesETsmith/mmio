# Grab all *.c files in the directory
file(GLOB TEST_SRC CONFIGURE_DEPENDS "*.c")

# file(GLOB TEST_DATA "${CMAKE_SOURCE_DIR}/data/*.mtx") file(COPY ${TEST_DATA}
# DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/.)

foreach(TEST_SRC_FILE ${TEST_SRC})
  # Strip file extension
  get_filename_component(TEST_NAME ${TEST_SRC_FILE} NAME_WLE)
  # Add test executable
  add_executable(${TEST_NAME} ${TEST_SRC_FILE})
  target_link_libraries(${TEST_NAME} PRIVATE mtxio Tau::Tau)
  add_test(${TEST_NAME} ${TEST_NAME})
  set_tests_properties(
    ${TEST_NAME} PROPERTIES ENVIRONMENT MTXIO_DATA_DIR=${CMAKE_SOURCE_DIR}/data)
endforeach()
