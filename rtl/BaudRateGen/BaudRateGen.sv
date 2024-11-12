module BaudRateGen #(
    int MaxClockRate = 100 * 10 ** 6,
    int MinBaudRate  = 9600,
    int Oversample   = 16
) (
    input clk,
    input nReset,
    input syncReset,

    input phase,
    input [txWidth-1:0] rate,

    output logic rxClk,
    output logic txClk
);

  localparam int txWidth = $clog2(MaxClockRate / MinBaudRate);
  localparam int rxShift = $clog2(Oversample);
  localparam int rxWidth = txWidth - rxShift;

  // Unreasonable to test these full-width
  /* verilator coverage_off */
  logic [txWidth-1:0] totalWait;
  logic [txWidth-1:0] postWait;
  logic [txWidth-1:0] preWait;
  /* verilator coverage_on */
  logic inWait;

  logic [rxWidth-1:0] rxRate;
  logic [rxWidth-1:0] offset;

  logic [rxWidth-1:0] rxCount;
  logic [txWidth-1:0] txCount;

  always_comb begin
    rxRate    = rate[txWidth-1:rxShift];
    offset    = rxRate - ((rxRate >> 1) + 1);

    totalWait = txWidth'(rate[rxShift-1:0]);
    preWait   = rate - (totalWait >> 1);
    postWait  = rate - preWait + txWidth'(rate[0]) + txWidth'(offset);
    inWait    = txCount > preWait || txCount < postWait;

    rxClk     = (rxRate > 1) ? (!inWait && rxCount == 0) ^ phase : phase;
    txClk     = (rate > 1) ? (txCount == 0) ^ phase : phase;
  end

  always @(posedge clk, negedge nReset) begin
    if (!nReset || syncReset || (txCount == 0)) begin
      rxCount <= rxRate - offset - 1;
    end else if (rxCount == 0) begin
      rxCount <= rxRate - 1;
    end else if (!inWait) begin
      rxCount <= rxCount - 1;
    end

    if (!nReset || syncReset || (txCount == 0)) begin
      txCount <= rate - 1;
    end else begin
      txCount <= txCount - 1;
    end
  end
endmodule
