module UartTxEn (
    input clk,
    input nReset,

    input en,
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

  logic hasData;
  logic enterStart;

  logic [7:0] writeBuf;
  logic [3:0] writeCount;

  always_comb begin
    done = en & (nextState == STOP);
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
      hasData    <= 0;
      enterStart <= 0;
    end else begin
      curState <= en ? nextState : curState;

      if (nextState == STOP || nextState == IDLE) begin
        if (valid) begin
          enterStart <= en ? 1 : enterStart;
          hasData    <= 1;
          writeCount <= 8;
          writeBuf   <= data;
        end else if (hasData) begin
          enterStart <= en ? 1 : enterStart;
        end
      end

      if (nextState == START) begin
        hasData <= en ? 0 : hasData;
        enterStart <= en ? 0 : enterStart;
      end

      if (nextState == DATA) begin
        writeCount <= en ? writeCount - 1 : writeCount;
        writeBuf   <= en ? 8'(writeBuf[7:1]) : writeBuf;
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
