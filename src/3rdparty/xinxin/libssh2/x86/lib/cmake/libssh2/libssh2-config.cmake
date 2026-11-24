# Copyright (C) The libssh2 project and its contributors.
# SPDX-License-Identifier: BSD-3-Clause

option(LIBSSH2_USE_CMAKECONFIG "Enable detecting libssh2 dependencies via CMake Config. Default: "
  "")
option(LIBSSH2_USE_PKGCONFIG "Enable pkg-config to detect libssh2 dependencies. Default: "
  "")

if(CMAKE_VERSION VERSION_LESS 3.18)
  message(STATUS "libssh2: libssh2-specific Find modules require "
    "CMake 3.18 or greater, found: ${CMAKE_VERSION}.")
endif()

include(CMakeFindDependencyMacro)

set(_libssh2_cmake_module_path_save ${CMAKE_MODULE_PATH})
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

set(_libssh2_libs "")
if("OpenSSL" STREQUAL "OpenSSL")
  find_dependency(OpenSSL)
elseif("OpenSSL" STREQUAL "wolfSSL")
  find_dependency(WolfSSL MODULE)
  list(APPEND _libssh2_libs libssh2::wolfssl)
elseif("OpenSSL" STREQUAL "Libgcrypt")
  find_dependency(Libgcrypt MODULE)
  list(APPEND _libssh2_libs libssh2::libgcrypt)
elseif("OpenSSL" STREQUAL "mbedTLS")
  find_dependency(MbedTLS MODULE)
  list(APPEND _libssh2_libs libssh2::mbedcrypto)
endif()

if()
  find_dependency(ZLIB)
endif()

set(CMAKE_MODULE_PATH ${_libssh2_cmake_module_path_save})

include("${CMAKE_CURRENT_LIST_DIR}/libssh2-targets.cmake")

# Alias for either shared or static library
if(NOT TARGET libssh2::ssh2)
  add_library(libssh2::ssh2 ALIAS libssh2::ssh2_shared)
endif()

# Compatibility alias
if(NOT TARGET Libssh2::ssh2)
  add_library(Libssh2::ssh2 ALIAS libssh2::ssh2_shared)
endif()
