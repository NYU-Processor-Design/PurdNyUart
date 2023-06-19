module EdgeSync (
    input clk,
    input nReset,

    input phase,
    input in,

    output logic out,
    output rise,
    output fall
);
  logic sync, cmp;

  assign rise = out & ~cmp;
  assign fall = ~out & cmp;

  always_ff @(posedge clk, negedge nReset) begin
    if (!nReset) begin
      cmp  <= phase;
      out  <= phase;
      sync <= phase;
    end else begin
      cmp  <= out;
      out  <= sync;
      sync <= in;
    end
  end

endmodule
