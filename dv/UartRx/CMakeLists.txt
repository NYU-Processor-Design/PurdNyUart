nyu_add_sv(tests SyncUartRx.sv SyncUartRxEn.sv)
target_sources(tests PRIVATE
  UartRx.cpp
  UartRxEn.cpp
)
