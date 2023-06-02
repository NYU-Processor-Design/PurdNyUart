module EdgeSync (
    input clk,

    input in,

    output rise,
    output fall,

    output logic out
);
  logic sync, cmp;

  assign rise = (out == 1) && (cmp == 0);
  assign fall = (out == 0) && (cmp == 1);

  always_ff @(posedge clk) begin
    cmp  <= out;
    out  <= sync;
    sync <= in;
  end

endmodule
