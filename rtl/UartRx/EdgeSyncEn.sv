module EdgeSyncEn (
    input clk,
    input nReset,

    input phase,
    input en,
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
      cmp  <= en ? out : cmp;
      out  <= en ? sync : out;
      sync <= en ? in : sync;
    end
  end

endmodule
