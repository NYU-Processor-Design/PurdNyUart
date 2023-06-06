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

  localparam int rxRate = ClockRate / (2 * BaudRate * Oversample);
  localparam int txRate = ClockRate / (2 * BaudRate);
  localparam int rxWidth = $clog2(rxRate);
  localparam int txWidth = $clog2(txRate);

  logic [rxWidth-1:0] rxCount;
  logic [txWidth-1:0] txCount;

  always @(posedge clk, negedge nReset) begin

    if (!nReset) begin
      rxClk   <= phase;
      txClk   <= phase;
      rxCount <= rxWidth'(rxRate);
      txCount <= txWidth'(txRate);
    end else begin

      // verilog_format: off
      if (rxCount > 0) begin
        rxCount <= rxCount - 1;
      end else if (
        (txCount > txWidth'(rxRate)) ||
        (txCount == 0)
      ) begin
        rxCount <= rxWidth'(rxRate);
        rxClk   <= ~rxClk;
      end
      // verilog_format: on

      if (txCount > 0) begin
        txCount <= txCount - 1;
      end else begin
        txCount <= txWidth'(txRate);
        txClk   <= ~txClk;
      end
    end

  end
endmodule