find_package(Catch2 3 REQUIRED CONFIG)
find_package(nyu-util REQUIRED CONFIG)

add_executable(tests)

add_subdirectory(BaudRateGen)
add_subdirectory(UartRx)
add_subdirectory(UartTx)
add_subdirectory(Loopback)

nyu_link_sv(tests PRIVATE uart)
nyu_target_verilate(tests
  TOP_MODULES BaudRateGen SyncUartRx SyncUartRxEn UartTx UartTxEn
  ARGS 
    ${_cov_option} 
    ${_traces_option}
  VERILATOR_ARGS -fno-table
)
nyu_target_verilate(tests
  TOP_MODULES Loopback_tb
)

if(NYU_GEN_COV)
  target_link_libraries(tests PRIVATE nyu::covrecorder)
endif()

target_link_libraries(tests PRIVATE
  Catch2::Catch2WithMain
  nyu::testutil
)
target_compile_features(tests PRIVATE cxx_std_23)

include(Catch)
catch_discover_tests(tests)
