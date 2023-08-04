module BaudRateGen #(
    int ClockRate  = 50 * 10 ** 6,
    int BaudRate   = 115200,
    int Oversample = 16
) (
    input clk,
    input nReset,
    input phase,

    output logic rxClk,
    output logic txClk
);

  localparam int rxRate = (ClockRate / (BaudRate * Oversample));
  localparam int txRate = (ClockRate / BaudRate);
  localparam int rxWidth = $clog2(rxRate);
  localparam int txWidth = $clog2(txRate);

  logic [rxWidth-1:0] rxCount;
  logic [txWidth-1:0] txCount;

  always_comb begin
    rxClk = rxCount > 0 ? phase : ~phase;
    txClk = txCount > 0 ? phase : ~phase;
  end

  always @(posedge clk, negedge nReset) begin
    if (!nReset) begin
      rxCount <= rxWidth'(rxRate - 1);
      txCount <= txWidth'(txRate - 1);
    end else begin
      txCount <= txCount > 0 ? txCount - 1 : txWidth'(txRate - 1);

      // verilog_format: off
      if (rxCount == 0) begin
        rxCount <= rxWidth'(rxRate - 1);
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
