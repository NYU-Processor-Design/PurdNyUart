module UartRx #(
    int Oversample = 16
) (
    input clk,
    input nReset,

    input in,

    output logic [7:0] data,

    output done,
    output err
);

  localparam sampleWidth = $clog2(Oversample);
  localparam fullSampleCount = sampleWidth'(Oversample - 1);
  localparam halfSampleCount = sampleWidth'((Oversample / 2) - 1);

  // verilog_format: off
  enum {
    IDLE,
    START,
    DATA_A,
    DATA_B,
    STOP,
    ERROR
  } curState , nextState;
  // verilog_format: on

  logic rise, fall, syncOut;

  EdgeSync es (
      .*,
      .out(syncOut)
  );

  logic edgeDetect;
  logic badSync;
  logic reSync;
  logic advance;

  always_comb begin
    edgeDetect = fall || rise;
    badSync = edgeDetect && edgeCmp && (sampleCount >= halfSampleCount);
    reSync = edgeDetect && (sampleCount < halfSampleCount);
    advance = reSync || (sampleCount == 0);
    done = advance && (readCount == 0);
    err = nextState == ERROR;
  end

  logic [sampleWidth-1:0] sampleCount;
  logic edgeCmp;

  always_ff @(posedge clk, negedge nReset) begin
    if (!nReset) begin
      sampleCount <= fullSampleCount;
      edgeCmp     <= 0;
      curState    <= IDLE;
    end else begin
      curState <= nextState;

      if (curState != nextState) begin
        edgeCmp     <= edgeDetect;
        sampleCount <= fullSampleCount;
      end else begin
        edgeCmp     <= edgeDetect ? edgeDetect : edgeCmp;
        sampleCount <= sampleCount - 1;
      end
    end
  end

  logic [7:0] readBuf;
  logic [3:0] readCount;

  always_ff @(posedge clk, negedge nReset) begin
    if (!nReset) begin
      readCount <= 8;
      data <= 0;
    end else begin

      if (readCount == 0) begin
        data <= readBuf;
      end

      if (nextState != DATA_A && nextState != DATA_B) begin
        readCount <= 8;
      end else if (sampleCount == halfSampleCount) begin
        readCount <= readCount - 1;
        readBuf   <= {readBuf[6:0], syncOut};
      end

    end
  end

  always_comb begin
    case (curState)

      IDLE:
      if (fall) begin
        nextState = START;
      end else begin
        nextState = curState;
      end

      START:
      if (badSync) begin
        nextState = ERROR;
      end else if (advance) begin
        nextState = DATA_A;
      end else begin
        nextState = curState;
      end

      DATA_A:
      if (badSync) begin
        nextState = ERROR;
      end else if (advance) begin
        nextState = readCount > 0 ? DATA_B : STOP;
      end else begin
        nextState = curState;
      end

      DATA_B:
      if (badSync) begin
        nextState = ERROR;
      end else if (advance) begin
        nextState = readCount > 0 ? DATA_A : STOP;
      end else begin
        nextState = curState;
      end

      STOP:
      if (
        badSync ||
        (rise && sampleCount <= halfSampleCount) ||
        (syncOut == 1 && sampleCount == halfSampleCount)
      ) begin
        nextState = ERROR;
      end else if (fall && sampleCount > halfSampleCount) begin
        nextState = START;
      end else if (advance) begin
        nextState = IDLE;
      end else begin
        nextState = curState;
      end

      ERROR: nextState = IDLE;

    endcase
  end

endmodule
