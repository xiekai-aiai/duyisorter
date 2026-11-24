#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libssh2::ssh2_static" for configuration "Release"
set_property(TARGET libssh2::ssh2_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libssh2::ssh2_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libssh2.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libssh2::ssh2_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_libssh2::ssh2_static "${_IMPORT_PREFIX}/lib/libssh2.a" )

# Import target "libssh2::ssh2_shared" for configuration "Release"
set_property(TARGET libssh2::ssh2_shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libssh2::ssh2_shared PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libssh2.so.1.0.1"
  IMPORTED_SONAME_RELEASE "libssh2.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS libssh2::ssh2_shared )
list(APPEND _IMPORT_CHECK_FILES_FOR_libssh2::ssh2_shared "${_IMPORT_PREFIX}/lib/libssh2.so.1.0.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
