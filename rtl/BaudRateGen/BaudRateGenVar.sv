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

  always @(posedge clk, negedge nReset) begin

    if (!nReset || syncReset) begin
      rxClk   <= phase;
      txClk   <= phase;
      rxCount <= rxRate;
      txCount <= txRate;
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
        rxCount <= rxRate;
        rxClk <= ~phase;
      end
      // verilog_format: on

      if (txCount > 0) begin
        txCount <= txCount - 1;
        if (txClk != phase) begin
          txClk <= phase;
        end
      end else begin
        txCount <= txRate;
        txClk   <= ~phase;
      end
    end

  end
endmodule
