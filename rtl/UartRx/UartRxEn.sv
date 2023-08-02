module UartRxEn #(
    int Oversample = 16
) (
    input clk,
    input nReset,

    input en,
    input in,

    output logic [7:0] data,

    output logic done,
    output logic err
);

  localparam sampleWidth = $clog2(Oversample);
  localparam fullSampleCount = sampleWidth'(Oversample - 1);
  localparam halfSampleCount = sampleWidth'(Oversample / 2);

  // verilog_format: off
  enum logic [2:0] {
    IDLE,
    START,
    DATA_A,
    DATA_B,
    STOP,
    ERROR
  } curState , nextState;
  // verilog_format: on

  logic rise, fall, syncOut;

  EdgeSyncEn es (
      .*,
      .phase(1'b1),
      .out  (syncOut)
  );

  logic edgeDetect;
  logic badSync;
  logic reSync;
  logic advance;

  logic [sampleWidth-1:0] sampleCount;
  logic [3:0] readCount;
  logic edgeCmp;

  always_comb begin
    edgeDetect = en ? fall || rise : 0;
    badSync = edgeDetect && edgeCmp && (sampleCount >= halfSampleCount);
    reSync = edgeDetect && (sampleCount < halfSampleCount);
    advance = reSync || (en && (sampleCount == 0));
    done = advance && (readCount == 0);
    err = nextState == ERROR;
  end

  always_ff @(posedge clk, negedge nReset) begin
    if (!nReset) begin
      sampleCount <= fullSampleCount;
      edgeCmp     <= 0;
      curState    <= IDLE;
    end else begin
      curState <= en ? nextState : curState;

      if (curState != nextState) begin
        edgeCmp     <= en ? edgeDetect : edgeCmp;
        sampleCount <= en ? fullSampleCount : sampleCount;
      end else begin
        edgeCmp     <= (en && edgeDetect) ? edgeDetect : edgeCmp;
        sampleCount <= en ? sampleCount - 1 : sampleCount;
      end
    end
  end

  logic [7:0] readBuf;

  always_ff @(posedge clk, negedge nReset) begin
    if (!nReset) begin
      readCount <= 8;
      data <= 0;
    end else begin

      if (readCount == 0) begin
        data <= en ? readBuf : data;
      end

      if (nextState != DATA_A && nextState != DATA_B) begin
        readCount <= en ? 8 : readCount;
      end else if (sampleCount == halfSampleCount) begin
        readCount <= en ? readCount - 1 : readCount;
        readBuf   <= en ? {syncOut, readBuf[7:1]} : readBuf;
      end

    end
  end

  always_comb begin

    nextState = curState;

    case (curState)

      IDLE:
      if (fall) begin
        nextState = START;
      end

      START:
      if (badSync) begin
        nextState = ERROR;
      end else if (advance) begin
        nextState = DATA_A;
      end

      DATA_A:
      if (badSync) begin
        nextState = ERROR;
      end else if (advance) begin
        nextState = readCount > 0 ? DATA_B : STOP;
      end

      DATA_B:
      if (badSync) begin
        nextState = ERROR;
      end else if (advance) begin
        nextState = readCount > 0 ? DATA_A : STOP;
      end

      STOP:
      if (badSync || (en && (syncOut == 0) && (sampleCount == halfSampleCount))) begin
        nextState = ERROR;
      end else if (en && fall && (sampleCount < halfSampleCount)) begin
        nextState = START;
      end else if (advance) begin
        nextState = IDLE;
      end

      // ERROR
      default: nextState = IDLE;

    endcase
  end

endmodule
