module UartTx (
    input clk,
    input nReset,

    input logic [7:0] data,
    input valid,

    output out,

    output busy,
    output done
);

  // verilog_format: off
  enum logic [1:0] {
    IDLE,
    START,
    DATA,
    STOP
  } curState, nextState;
  // verilog_format: on

  logic [7:0] writeBuf;
  logic [3:0] writeCount;
  logic enterStart;

  always_comb begin
    done = nextState == STOP;
    busy = nextState != IDLE;
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
      enterStart <= 0;
    end else begin
      curState <= nextState;

      if ((nextState == STOP || nextState == IDLE) && valid) begin
        enterStart <= 1;
        writeCount <= 8;
        writeBuf   <= data;
      end

      if (nextState == START) begin
        enterStart <= 0;
      end

      if (nextState == DATA) begin
        writeCount <= writeCount - 1;
        writeBuf   <= 8'(writeBuf[7:1]);
      end

    end
  end

  always_comb begin
    case (curState)
      IDLE: nextState = enterStart ? START : curState;

      START: nextState = DATA;

      DATA: nextState = |writeCount ? curState : STOP;

      STOP: nextState = enterStart ? START : IDLE;
    endcase
  end

endmodule
