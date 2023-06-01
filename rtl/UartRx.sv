module UartRx #(
    int Oversample = 16
) (
    input rxClk,
    input en,
    input in,

    output logic [7:0] data,
    output logic done,
    output logic busy,
    output logic err
);

endmodule
