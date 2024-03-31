function(create_target PROJECT_NAME)
  foreach(interface IN LISTS interfaces)
    string(JOIN "" path "${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}/${interface}")
    list(APPEND headers "${path}")
  endforeach()
  set(exe_sources ${main_source} ${sources})

  #
  # Setup alternative names
  #

  if(${PROJECT_NAME}_USE_ALT_NAMES)
    string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
    string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPERCASE)
  else()
    set(PROJECT_NAME_LOWERCASE ${PROJECT_NAME})
    set(PROJECT_NAME_UPPERCASE ${PROJECT_NAME})
  endif()

  #
  # Create library, setup header and source files
  #

  if(${PROJECT_NAME}_BUILD_EXECUTABLE)
    add_executable(${PROJECT_NAME} ${exe_sources})

    if(VERBOSE_OUTPUT)
      verbose_message("Found the following sources:")
      foreach(source IN LISTS exe_sources)
        verbose_message("* ${source}")
      endforeach()
    endif()

    if(${PROJECT_NAME}_ENABLE_UNIT_TESTING)
      add_library(${PROJECT_NAME}_LIB ${headers} ${sources})

      if(VERBOSE_OUTPUT)
        verbose_message("Found the following headers:")
        foreach(header IN LISTS headers)
          verbose_message("* ${header}")
        endforeach()
      endif()
    endif()
  elseif(${PROJECT_NAME}_BUILD_HEADERS_ONLY)
    add_library(${PROJECT_NAME} INTERFACE)

    if(VERBOSE_OUTPUT)
      verbose_message("Found the following headers:")
      foreach(header IN LISTS headers)
        verbose_message("* ${header}")
      endforeach()
    endif()
  else()
    add_library(
      ${PROJECT_NAME}
      ${headers}
      ${sources}
    )

    if(VERBOSE_OUTPUT)
      verbose_message("Found the following sources:")
      foreach(source IN LISTS sources)
        verbose_message("* ${source}")
      endforeach()
      verbose_message("Found the following headers:")
      foreach(header IN LISTS headers)
        verbose_message("* ${header}")
      endforeach()
    endif()
  endif()

  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
  )
  if(${PROJECT_NAME}_BUILD_EXECUTABLE AND ${PROJECT_NAME}_ENABLE_UNIT_TESTING)
    set_target_properties(
      ${PROJECT_NAME}_LIB
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
      OUTPUT_NAME ${PROJECT_NAME}
    )
  endif()

  message(STATUS "Added all header and implementation files.\n")

  #
  # Set the project standard and warnings
  #

  if(${PROJECT_NAME}_BUILD_HEADERS_ONLY)
    target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
  else()
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

    if(${PROJECT_NAME}_BUILD_EXECUTABLE AND ${PROJECT_NAME}_ENABLE_UNIT_TESTING)
      target_compile_features(${PROJECT_NAME}_LIB PUBLIC cxx_std_17)
    endif()
  endif()
  set_project_warnings(${PROJECT_NAME})

  verbose_message("Applied compiler warnings. Using standard ${CMAKE_CXX_STANDARD}.\n")

  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

  verbose_message("Successfully added all dependencies and linked against them.")

  #
  # Set the build/user include directories
  #

  # Allow usage of header files in the `src` directory, but only for utilities
  if(${PROJECT_NAME}_BUILD_HEADERS_ONLY)
    target_include_directories(
      ${PROJECT_NAME}
      INTERFACE
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}>
    )
  else()
    target_include_directories(
      ${PROJECT_NAME}
      PUBLIC
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}>
      PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    if(${PROJECT_NAME}_BUILD_EXECUTABLE AND ${PROJECT_NAME}_ENABLE_UNIT_TESTING)
      target_include_directories(
        ${PROJECT_NAME}_LIB
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
      )
    endif()
  endif()

  message(STATUS "Finished setting up include directories.")

  #
  # Provide alias to library for
  #

  if(${PROJECT_NAME}_BUILD_EXECUTABLE)
    add_executable(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
  else()
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
  endif()

  verbose_message("Project is now aliased as ${PROJECT_NAME}::${PROJECT_NAME}.\n")

  #
  # Install library for easy downstream inclusion
  #

  include(GNUInstallDirs)
  install(
    TARGETS
    ${PROJECT_NAME}
    EXPORT
    ${PROJECT_NAME}Targets
    LIBRARY DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION
    include
    PUBLIC_HEADER DESTINATION
    include
  )

  install(
    EXPORT
    ${PROJECT_NAME}Targets
    FILE
    ${PROJECT_NAME}Targets.cmake
    NAMESPACE
    ${PROJECT_NAME}::
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )

  #
  # Add version header
  #

  configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/version.hpp.in
    include/${PROJECT_NAME_LOWERCASE}/version.hpp
    @ONLY
  )

  install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME_LOWERCASE}/version.hpp
    DESTINATION
    include/${PROJECT_NAME_LOWERCASE}
  )

  #
  # Install the `include` directory
  #

  install(
    DIRECTORY
    include/${PROJECT_NAME_LOWERCASE}
    DESTINATION
    include
  )

  verbose_message("Install targets succesfully build. Install with `cmake --build <build_directory> --target install --config <build_config>`.")

  #
  # Quick `ConfigVersion.cmake` creation
  #

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    ${PROJECT_NAME}ConfigVersion.cmake
    VERSION
    ${PROJECT_VERSION}
    COMPATIBILITY
    SameMajorVersion
  )

  configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/ProjectConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )

  install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )

  #
  # Generate export header if specified
  #

  if(${PROJECT_NAME}_GENERATE_EXPORT_HEADER)
    include(GenerateExportHeader)
    generate_export_header(${PROJECT_NAME})
    install(
      FILES
      ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWERCASE}_export.h
      DESTINATION
      include
    )

    message(STATUS "Generated the export header `${PROJECT_NAME_LOWERCASE}_export.h` and installed it.")
  endif()

  message(STATUS "Finished building requirements for installing the package.\n")

  #
  # Unit testing setup
  #

  if(${PROJECT_NAME}_ENABLE_UNIT_TESTING)
    enable_testing()
    message(STATUS "Build unit tests for the project. Tests should always be found in the test folder\n")
    add_subdirectory(${CMAKE_SOURCE_DIR}/test/${PROJECT_NAME} test SYSTEM)
  endif()
endfunction()
