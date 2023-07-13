find_package(nyu-cmake CONFIG REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/purdnyuartTargets.cmake)

nyu_include_fixup(nyu::uart)
