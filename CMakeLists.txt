cmake_minimum_required(VERSION 3.24)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  include(FetchContent)
  FetchContent_Declare(
    vcpkg
    GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
    GIT_TAG master
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(vcpkg)
  set(CMAKE_TOOLCHAIN_FILE
    ${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake
    CACHE FILEPATH "Vcpkg toolchain file"
  )
  set(VCPKG_ROOT_DIR ${vcpkg_SOURCE_DIR} CACHE PATH "Vcpkg Root Directory")
endif()

add_custom_target(UpdateVcpkgBaseline
  ${VCPKG_ROOT_DIR}/vcpkg x-update-baseline
)

project(purdnyuart VERSION 1.0.0)

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
