module UartTx (
    input clk,
    input nReset,

    input logic [7:0] data,
    input valid,

    output logic out,

    output busy,
    output done
);

  // verilog_format: off
  enum {
    IDLE,
    START,
    DATA,
    STOP
  } curState , nextState;
  // verilog_format: on

  logic [7:0] writeBuf;
  logic [3:0] writeCount;

  always_comb begin
    done = writeCount == 0;
    busy = curState != IDLE;
  end

  always_comb begin
    if (nextState == DATA) begin
      out = writeBuf[0];
    end else if (nextState == START) begin
      out = 0;
    end else begin
      out = 1;
    end
  end

  always_ff @(posedge clk, negedge nReset) begin
    if (!nReset) begin
      curState   <= IDLE;
      writeCount <= 8;
      writeBuf   <= 0;
    end else begin
      curState <= nextState;

      if (!busy && valid) begin
        writeCount <= 8;
        writeBuf   <= data;
      end

      if (nextState == DATA) begin
        writeCount <= writeCount - 1;
        writeBuf   <= 8'(writeBuf[7:1]);
      end

    end
  end

  always_comb begin
    case (curState)
      IDLE: nextState = valid ? START : curState;

      START: nextState = DATA;

      DATA: nextState = |writeCount ? curState : STOP;

      STOP: nextState = IDLE;
    endcase
  end

endmodule
