if(ENABLE_CONAN)
  #
  # Setup Conan requires and options here:
  #

  list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
  list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

  #
  # If `conan.cmake` (from https://github.com/conan-io/cmake-conan) does not exist, download it.
  #
  set(CONAN_CMAKE_FILE "${CMAKE_BINARY_DIR}/conan.cmake")
  if(NOT EXISTS "${CONAN_CMAKE_FILE}")
    message(
      STATUS
      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan..."
    )
    file(
      DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/0.18.1/conan.cmake"
      "${CONAN_CMAKE_FILE}"
    )

    message(STATUS "Cmake-Conan downloaded succesfully.")
  endif()

  include(${CONAN_CMAKE_FILE})

  conan_cmake_configure(
    REQUIRES catch2/3.5.3
    GENERATORS CMakeDeps
  )

  conan_cmake_autodetect(settings)

  conan_cmake_install(
    PATH_OR_REFERENCE .
    BUILD missing
    REMOTE conancenter
    SETTINGS ${settings}
  )

  verbose_message("Conan is setup and all requires have been installed.")
endif()
