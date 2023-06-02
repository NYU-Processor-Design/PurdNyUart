module BaudRateGenVar #(
    int MaxClockRate = 100 * 10 ** 6,
    int MinBaudRate  = 9600,
    int Oversample   = 16
) (
    input clk,
    input nReset,
    input phase,

    input [txWidth-1:0] count,

    output logic rxClk,
    output logic txClk
);

  localparam int txWidth = $clog2(MaxClockRate / (2 * MinBaudRate));
  localparam int rxShift = $clog2(Oversample);
  localparam int rxWidth = txWidth - rxShift;

  logic [rxWidth-1:0] rxCount;
  logic [txWidth-1:0] txCount;

  always @(posedge clk, negedge nReset) begin

    if (!nReset) begin
      rxClk   <= phase;
      txClk   <= phase;
      rxCount <= count[txWidth-1:rxShift];
      txCount <= count;
    end else begin

      // verilog_format: off
      if (rxCount > 0) begin
        rxCount <= rxCount - 1;
      end else if (
        (txCount > txWidth'(count[txWidth-1:rxShift])) ||
        (txCount == 0)
      ) begin
        rxCount <= count[txWidth-1:rxShift];
        rxClk   <= ~rxClk;
      end
      // verilog_format: on

      if (txCount > 0) begin
        txCount <= txCount - 1;
      end else begin
        txCount <= count;
        txClk   <= ~txClk;
      end
    end

  end
endmodule
