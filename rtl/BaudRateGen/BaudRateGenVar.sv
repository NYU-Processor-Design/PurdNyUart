module BaudRateGenVar #(
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

  logic [txWidth-1:0] postWait;
  logic [txWidth-1:0] preWait;
  logic inWait;

  logic [rxWidth-1:0] rxRate;

  logic [rxWidth-1:0] rxCount;
  logic [txWidth-1:0] txCount;

  always_comb begin
    rxRate   = rate[txWidth-1:rxShift];
    postWait = txWidth'(rate[rxShift-1:1]) + txWidth'(rate[0]);
    preWait  = rate - txWidth'(rate[rxShift-1:1]) - 1;
    inWait   = txCount > preWait || txCount < postWait;

    if (!nReset || syncReset) begin
      rxClk = rate == txWidth'(Oversample) ? ~phase : phase;
    end else if (rxRate > 1) begin
      rxClk = inWait ? phase : rxCount == 0;
    end else begin
      rxClk = inWait ? phase : (clk ^ phase) & (rxCount == 0);
    end

    if (rate > 1) begin
      txClk = (txCount == 0) ^ phase;
    end else begin
      txClk = clk ^ phase;
    end
  end

  always @(posedge clk, negedge nReset) begin

    if (!nReset || syncReset || (txCount == 0)) begin
      txCount <= rate - 1;
    end else begin
      txCount <= txCount - 1;
    end

    if (!nReset || syncReset || (rxCount == 0)) begin
      rxCount <= rxRate - 1;
      rxCount <= rxRate - 1;
    end else if (!inWait) begin
      rxCount <= rxCount - 1;
    end
  end
endmodule
