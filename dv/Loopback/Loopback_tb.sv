module Loopback_tb (
    input clk,
    input nReset,

    input logic [7:0] data_tx,
    input valid,
    output busy,
    output done_tx,

    output logic [7:0] data_rx,
    output done_rx,
    output err
);

  logic rxClk;
  logic txClk;

  BaudRateGen bg (
      .phase(0),
      .syncReset(0),
      .rate(2 << 4),
      .*
  );

  logic tx_out;
  logic loopback;

  always_ff @(posedge clk, negedge nReset) begin
    if (!nReset) loopback <= 1;
    else loopback <= tx_out;
  end


  UartTxEn tx (
      .en  (txClk),
      .data(data_tx),
      .out (tx_out),
      .done(done_tx),
      .*
  );

  UartRxEn rx (
      .en  (rxClk),
      .data(data_rx),
      .in  (loopback),
      .done(done_rx),
      .*
  );

endmodule
