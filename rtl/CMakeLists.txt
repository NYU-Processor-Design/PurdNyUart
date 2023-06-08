add_subdirectory(BaudRateGen)

nyu_add_sv(uart
  EdgeSync.sv
  UartRx.sv
  UartTx.sv
)
