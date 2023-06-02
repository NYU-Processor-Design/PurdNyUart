module EdgeSync (
    input clk,
    input nReset,

    input in,

    output rise,
    output fall,

    output logic out
);
  logic sync, cmp;

  assign rise = out & ~cmp;
  assign fall = ~out & cmp;

  always_ff @(posedge clk, negedge nReset) begin
    if (!nReset) begin
      cmp  <= 0;
      out  <= 0;
      sync <= 0;
    end else begin
      cmp  <= out;
      out  <= sync;
      sync <= in;
    end
  end

endmodule
