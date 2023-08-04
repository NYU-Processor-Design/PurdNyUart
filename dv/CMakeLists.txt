find_package(Catch2 3 REQUIRED CONFIG)
find_package(nyu-util REQUIRED CONFIG)

add_executable(tests)

add_subdirectory(BaudRateGen)
add_subdirectory(UartRx)
add_subdirectory(UartTx)
add_subdirectory(Loopback)

nyu_link_sv(tests PRIVATE uart)
nyu_target_verilate(tests
  TOP_MODULES BaudRateGen BaudRateGen4 BaudRateGenVar
  EdgeSync EdgeSyncEn UartRx UartRxEn UartTx UartTxEn
  ARGS COVERAGE TRACE_FST
)
nyu_target_verilate(tests
  TOP_MODULES Loopback_tb
)
target_link_libraries(tests PRIVATE
  Catch2::Catch2WithMain
  nyu::covrecorder
  nyu::testutil
)
target_compile_features(tests PRIVATE cxx_std_23)

include(Catch)
catch_discover_tests(tests)
