cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(algorithms VERSION 0.1 LANGUAGES CXX)

#
# Catch2 dependency
#

include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    Catch2Project
    PREFIX ${CMAKE_BINARY_DIR}/Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.2.1
    TIMEOUT 10
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
   )

ExternalProject_Get_Property(Catch2Project source_dir) # TOOD source_dir should not be in this scope
add_library(Catch2 INTERFACE IMPORTED)
add_dependencies(Catch2 Catch2Project)
file(MAKE_DIRECTORY ${source_dir}/single_include) # TODO this is a workaround for https://public.kitware.com/Bug/view.php?id=15052
#set_property(TARGET Catch2 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/single_include)

set_property(TARGET Catch2 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  $<BUILD_INTERFACE:${source_dir}/single_include>)

#
# Test Functionality
#

enable_testing()


function(add_catch2_test TEST_EXECUTABLE_NAME SOURCE_FILE_NAME)
  set(DEPENDENCY_LIB ${ARGV2})

  add_executable(${TEST_EXECUTABLE_NAME} ${SOURCE_FILE_NAME})

  target_link_libraries(${TEST_EXECUTABLE_NAME} 
    ${DEPENDENCY_LIB} Catch2 common)

  add_test(
    NAME "${TEST_EXECUTABLE_NAME}"
    COMMAND ${TEST_EXECUTABLE_NAME})
endfunction()

#
# Dual Use Functionality (test + main)
#

function(add_dual SOURCE_FILE_NAME)
  get_filename_component(EXECUTABLE_NAME ${SOURCE_FILE_NAME} NAME_WE)
  
  add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE_NAME})
  target_compile_definitions(${EXECUTABLE_NAME} PRIVATE COMPILE_MAIN=1)
  target_link_libraries(${EXECUTABLE_NAME} common Catch2)

  set(TEST_EXECUTABLE_NAME "test_${EXECUTABLE_NAME}")
  add_catch2_test(${TEST_EXECUTABLE_NAME} ${SOURCE_FILE_NAME})
  target_compile_definitions(${TEST_EXECUTABLE_NAME} PRIVATE COMPILE_TESTS=1)

  # TODO output junit file?
endfunction()

#
# Common library
#

add_library(common INTERFACE)
target_compile_options(common INTERFACE -Wall -Wextra -std=c++2a)
target_include_directories(common INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    #PRIVATE src
    )

add_executable(sudoku "main.cpp")

add_subdirectory(ioi)
