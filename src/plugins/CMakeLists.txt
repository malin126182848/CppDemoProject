cmake_minimum_required(VERSION 3.15)

project(
  plugins
  VERSION
  ${PROJECT_VERSION}
  LANGUAGES
  CXX
)

#
# Project settings
#

option(${PROJECT_NAME}_BUILD_EXECUTABLE "Build the project as an executable, rather than a library." OFF)
option(${PROJECT_NAME}_BUILD_HEADERS_ONLY "Build the project as a header-only library." OFF)
option(${PROJECT_NAME}_USE_ALT_NAMES "Use alternative names for the project, such as naming the include directory all lowercase." ON)
option(${PROJECT_NAME}_ENABLE_UNIT_TESTING "Enable unit tests for the projects (from the `test` subfolder)." ON)
option(${PROJECT_NAME}_GENERATE_EXPORT_HEADER "Create a `project_export.h` file containing all exported symbols." OFF)

set(sources
  Plugin.cpp
)

set(main_source main.cpp)

set(interfaces
  IPlugin.h
)

set(test_sources
  src/tmp_test.cpp
)

create_target(${PROJECT_NAME})
