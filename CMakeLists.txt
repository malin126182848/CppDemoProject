cmake_minimum_required(VERSION 3.15)
#
# Project details
#

project(
  "Project"
  VERSION 0.1.0
  LANGUAGES CXX
)
message(STATUS "Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...\n")

#
# Set project options
#
include(cmake/StandardSettings.cmake)
include(cmake/StaticAnalyzers.cmake)
include(cmake/Utils.cmake)
message(STATUS ${CMAKE_CONFIGURATION_TYPES})
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

if(UNIX)
  add_compile_options("$<$<CONFIG:DEBUG>:-D_DEBUG>")
  # this will allow to use same _DEBUG macro available in both Linux
  # as well as Windows - MSVC environment. Easy to put Debug specific code.
endif()

#
# Prevent building in the source directory
#

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n")
endif()

#
# Enable package managers
#
include(cmake/Conan.cmake)
include(cmake/Vcpkg.cmake)

#
# Enable Doxygen
#
include(cmake/Doxygen.cmake)

#
# Format the project using the `clang-format` target (i.e: cmake --build build --target clang-format)
#
add_clang_format_target()

#
# Set the project standard and warnings
#
include(cmake/CompilerWarnings.cmake)

#
# Set the create target function
#
include(cmake/CreateTarget.cmake)

# add_subdirectory(src/plugins)
add_subdirectory(src/event)
