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

  localparam int rxRate = (ClockRate / (BaudRate * Oversample)) - 1;
  localparam int txRate = (ClockRate / BaudRate) - 1;
  localparam int rxWidth = $clog2(rxRate + 1);
  localparam int txWidth = $clog2(txRate + 1);

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
        if(rxClk != phase) begin
          rxClk <= phase;
        end
      end else if (
        (txCount > txWidth'(rxRate)) ||
        (txCount == 0)
      ) begin
        rxCount <= rxWidth'(rxRate);
        rxClk <= ~phase;
      end
      // verilog_format: on

      if (txCount > 0) begin
        txCount <= txCount - 1;
        if (txClk != phase) begin
          txClk <= phase;
        end
      end else begin
        txCount <= txWidth'(txRate);
        txClk   <= ~phase;
      end
    end

  end
endmodule
