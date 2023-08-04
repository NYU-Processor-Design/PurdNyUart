module BaudRateGenVar #(
    int rxWidth = 16,
    int txWidth = 16
) (
    input clk,
    input nReset,
    input syncReset,
    input phase,

    input [rxWidth-1:0] rxRate,
    input [txWidth-1:0] txRate,

    output logic rxClk,
    output logic txClk
);

  logic [rxWidth-1:0] rxCount;
  logic [txWidth-1:0] txCount;

  always_comb begin
    rxClk = rxCount > 0 ? phase : ~phase;
    txClk = txCount > 0 ? phase : ~phase;
  end

  always @(posedge clk, negedge nReset) begin

    if (!nReset || syncReset) begin
      rxCount <= rxRate - 1;
      txCount <= txRate - 1;
    end else begin
      txCount <= txCount > 0 ? txCount - 1 : txRate - 1;

      // verilog_format: off
      if (rxCount == 0) begin
        rxCount <= rxRate - 1;
      end else if (
        (rxCount > 1) ||
        (txCount > txWidth'(rxRate)) ||
        (txCount == 1)
      ) begin
        rxCount <= rxCount - 1;
      end
      // verilog_format: on
    end

  end
endmodule
