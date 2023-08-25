module SyncUartRx #(
    int Oversample = 16
) (
    input clk,
    input nReset,

    input in,

    output logic [7:0] data,

    output logic done,
    output logic err
);

  logic sync;
  always_ff @(posedge clk, negedge nReset) sync <= !nReset ? 1 : in;

  UartRx #(Oversample) rx (
      .in(sync),
      .*
  );

endmodule
