module BaudRateGen4 #(
    int ClockRate = 50 * 10 ** 6,
    int Baud1 = 9600,
    int Baud2 = 38400,
    int Baud3 = 57600,
    int Baud4 = 115200,
    int Oversample = 16
) (
    input clk,
    input nReset,
    input phase,

    input [1:0] sel,

    output logic rxClk,
    output logic txClk
);

  `define calcRx(baud) (ClockRate /  ((baud) * Oversample))
  `define calcTx(baud) (ClockRate / (baud))

  `define max2(a, b) ((a) > (b) ? (a) : (b))
  `define max3(a, b, c) `max2((a), `max2((b), (c)))
  `define max4(a, b, c, d) `max3((a), (b), `max2((c), (d)))

  // verilog_format: off
  localparam int rx1 = `calcRx(Baud1);
  localparam int tx1 = `calcTx(Baud1);

  localparam int rx2 = `calcRx(Baud2);
  localparam int tx2 = `calcTx(Baud2);

  localparam int rx3 = `calcRx(Baud3);
  localparam int tx3 = `calcTx(Baud3);

  localparam int rx4 = `calcRx(Baud4);
  localparam int tx4 = `calcTx(Baud4);

  localparam int maxRx = `max4(rx1, rx2, rx3, rx4);
  localparam int maxTx = `max4(tx1, tx2, tx3, tx4);

  localparam int rxArr[4] = '{rx1, rx2, rx3, rx4};
  localparam int txArr[4] = '{tx1, tx2, tx3, tx4};
  // verilog_format: on

  `undef calcRx
  `undef calcTx
  `undef max2
  `undef max3
  `undef max4

  localparam int rxWidth = $clog2(maxRx);
  localparam int txWidth = $clog2(maxTx);

  logic [rxWidth-1:0] rxCount;
  logic [txWidth-1:0] txCount;

  always_comb begin
    rxClk = rxCount > 0 ? phase : ~phase;
    txClk = txCount > 0 ? phase : ~phase;
  end

  always @(posedge clk, negedge nReset) begin

    if (!nReset) begin
      rxCount <= rxWidth'(rxArr[sel] - 1);
      txCount <= txWidth'(txArr[sel] - 1);
    end else begin
      txCount <= txCount > 0 ? txCount - 1 : txWidth'(txArr[sel] - 1);

      // verilog_format: off
      if (rxCount == 0) begin
        rxCount <= rxWidth'(rxArr[sel] - 1);
      end else if (
        (rxCount > 1) ||
        (txCount > txWidth'(rxArr[sel])) ||
        (txCount == 1)
      ) begin
        rxCount <= rxCount - 1;
      end
      // verilog_format: on
    end
  end
endmodule
