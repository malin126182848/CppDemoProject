cmake_minimum_required(VERSION 3.15)

#
# Project details
#

set(TEST_LIB_NAME ${PROJECT_NAME})
project(
  ${TEST_LIB_NAME}Tests
  LANGUAGES CXX
)

verbose_message("Adding tests under ${TEST_LIB_NAME}Tests...")

foreach(file ${test_sources})
  string(REGEX REPLACE "(.*/)([a-zA-Z0-9_ ]+)(\.cpp)" "\\2" test_name ${file})
  add_executable(${test_name}_Tests ${file})

  #
  # Set the compiler standard
  #

  target_compile_features(${test_name}_Tests PUBLIC cxx_std_17)

  #
  # Setup code coverage if enabled
  #

  if(ENABLE_CODE_COVERAGE)
    target_compile_options(${TEST_LIB_NAME} PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
    target_link_options(${TEST_LIB_NAME} PUBLIC -fprofile-arcs -ftest-coverage)
    verbose_message("Code coverage is enabled and provided with GCC.")
  endif()

  #
  # Load the desired unit testing framework
  #
  # Currently supported: GoogleTest (and GoogleMock), Catch2.

  if(${TEST_LIB_NAME}_BUILD_EXECUTABLE)
    set(${TEST_LIB_NAME}_TEST_LIB ${TEST_LIB_NAME}_LIB)
  else()
    set(${TEST_LIB_NAME}_TEST_LIB ${TEST_LIB_NAME})
  endif()

  message(STATUS "Testing library: ${${TEST_LIB_NAME}_TEST_LIB}")

  if(USE_GTEST)
    find_package(GTest REQUIRED)

    if(USE_GOOGLE_MOCK)
      set(GOOGLE_MOCK_LIBRARIES GTest::gmock GTest::gmock_main)
    endif()

    target_link_libraries(
      ${test_name}_Tests
      PUBLIC
        GTest::GTest
        GTest::Main
        ${GOOGLE_MOCK_LIBRARIES}
        ${${TEST_LIB_NAME}_TEST_LIB}
    )
  elseif(USE_CATCH2)
    find_package(Catch2 REQUIRED)
    target_link_libraries(
      ${test_name}_Tests
      PUBLIC
        Catch2::Catch2WithMain
        ${${TEST_LIB_NAME}_TEST_LIB}
    )
  else()
    message(FATAL_ERROR "Unknown testing library. Please setup your desired unit testing library by using `target_link_libraries`.")
  endif()

  #
  # Add the unit tests
  #

  add_test(
    NAME
      ${test_name}
    COMMAND
      ${test_name}_Tests
  )
endforeach()

verbose_message("Finished adding unit tests for ${TEST_LIB_NAME}.")
