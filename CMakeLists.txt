cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(algorithms VERSION 0.1 LANGUAGES CXX)

find_package(Boost REQUIRED unit_test_framework)

enable_testing()

function(add_boost_test TEST_EXECUTABLE_NAME SOURCE_FILE_NAME)
  set(DEPENDENCY_LIB ${ARGV2})

  add_executable(${TEST_EXECUTABLE_NAME} ${SOURCE_FILE_NAME})

  target_link_libraries(${TEST_EXECUTABLE_NAME} 
    ${DEPENDENCY_LIB} Boost::unit_test_framework common)

  add_test(
    NAME "${TEST_EXECUTABLE_NAME}"
    COMMAND ${TEST_EXECUTABLE_NAME} --catch_system_error=yes)
  # TODO  --log_format=JUNIT --log_level=all --log_sink=${EXECUTABLE_NAME}.xml
endfunction()

function(add_dual SOURCE_FILE_NAME)
  get_filename_component(EXECUTABLE_NAME ${SOURCE_FILE_NAME} NAME_WE)
  
  add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE_NAME})
  target_compile_definitions(${EXECUTABLE_NAME} PRIVATE PROGRAM_NAME=${EXECUTABLE_NAME} EXECUTABLE_NAME COMPILE_MAIN=1)
  target_link_libraries(${EXECUTABLE_NAME} common Boost::unit_test_framework)

  set(TEST_EXECUTABLE_NAME "test_${EXECUTABLE_NAME}")
  add_boost_test(${TEST_EXECUTABLE_NAME} ${SOURCE_FILE_NAME})
  target_compile_definitions(${TEST_EXECUTABLE_NAME} PRIVATE PROGRAM_NAME=${EXECUTABLE_NAME} EXECUTABLE_NAME COMPILE_TESTS=1)

  # TODO output junit file?
endfunction()

add_library(common INTERFACE)
target_compile_features(common INTERFACE cxx_std_14)
target_compile_options(common INTERFACE -Wall -Wextra -std=c++2a)
target_include_directories(common INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    #PRIVATE src
    )

# TODO move into sub_directory
add_executable(sudoku "main.cpp")

add_subdirectory(ioi)
