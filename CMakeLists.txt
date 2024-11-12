cmake_minimum_required(VERSION 3.27)

option(NYU_SKIP_VCPKG "Do not fetch vcpkg" OFF)
option(NYU_BUILD_TESTS "Generate tests" OFF)
option(NYU_GEN_TRACES "Generate FST traces from tests" OFF)
option(NYU_GEN_COV "Generate coverage information from tests" OFF)

if(NYU_GEN_TRACES)
  set(_traces_option TRACE_FST)
else()
  set(_traces_option)
endif()

if(NYU_GEN_COV)
  set(_cov_option COVERAGE)
else()
  set(_cov_option)
endif()

include(cmake/BootstrapVcpkg.cmake)

project(purdnyuart VERSION 2.0.2)

find_package(nyu-cmake CONFIG REQUIRED)

add_library(uart INTERFACE)

add_subdirectory(rtl)

if(NYU_BUILD_TESTS)
  enable_testing()
  add_subdirectory(dv)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

nyu_install_sv(
  EXPORT purdnyuartTargets
  TARGETS uart
  NAMESPACE nyu::
  EXPORT_DEST ${CMAKE_INSTALL_DATADIR}/purdnyuart
  SV_DEST ${CMAKE_INSTALL_DATADIR}/purdnyuart/rtl
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/purdnyuart-config-version.cmake
  COMPATIBILITY ExactVersion
  ARCH_INDEPENDENT
)

install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/purdnyuart-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/purdnyuart-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/purdnyuart
)
