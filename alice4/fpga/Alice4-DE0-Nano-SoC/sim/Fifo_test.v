
module Fifo_test(
    input clock
);

    localparam STATE_INIT = 4'h0;
    localparam STATE_WRITE1 = 4'h1;
    localparam STATE_WRITE2 = 4'h2;
    localparam STATE_WRITE3 = 4'h3;
    localparam STATE_PAUSE = 4'h4;
    localparam STATE_READ1 = 4'h5;
    localparam STATE_READ2 = 4'h6;
    localparam STATE_READ3 = 4'h7;
    localparam STATE_HALT = 4'h8;
    reg [3:0] state /* verilator public */;

    localparam FIFO_DEPTH = 256;
    localparam FIFO_DEPTH_LOG2 = 8;
    reg fifo_sclr;
    wire fifo_full /* verilator public */;
    wire fifo_empty /* verilator public */;
    reg fifo_rdreq /* verilator public */;
    reg fifo_wrreq /* verilator public */;
    reg [63:0] fifo_data /* verilator public */;
    wire [63:0] fifo_q /* verilator public */;
    wire [FIFO_DEPTH_LOG2 - 1:0] fifo_usedw /* verilator public */;
    /* verilator lint_off PINMISSING */
    scfifo #(.add_ram_output_register("OFF"),
             .intended_device_family("CYCLONEV"),
             .lpm_numwords(FIFO_DEPTH),
             .lpm_showahead("OFF"),
             .lpm_type("scfifo"),
             .lpm_width(64),
             .lpm_widthu(FIFO_DEPTH_LOG2),
             .overflow_checking("ON"),
             .underflow_checking("ON"),
             .use_eab("ON")) fifo(
            .aclr(1'b0),
            .sclr(fifo_sclr),
            .clock(clock),
            .data(fifo_data),
            .empty(fifo_empty),
            .full(fifo_full),
            .usedw(fifo_usedw),
            .q(fifo_q),
            .rdreq(fifo_rdreq),
            .wrreq(fifo_wrreq));
    /* verilator lint_on PINMISSING */

    initial begin
        fifo_sclr = 1'b0;
        fifo_rdreq = 1'b0;
        fifo_wrreq = 1'b0;
        fifo_data = 64'h0;
        state = STATE_INIT;
    end

    always @(posedge clock) begin
        case (state)
            STATE_INIT: begin
                state <= STATE_WRITE1;
            end

            STATE_WRITE1: begin
                fifo_wrreq <= 1'b1;
                fifo_data <= 64'hA;
                state <= STATE_WRITE2;
            end

            STATE_WRITE2: begin
                fifo_data <= 64'hB;
                state <= STATE_WRITE3;
            end

            STATE_WRITE3: begin
                fifo_data <= 64'hC;
                state <= STATE_PAUSE;
            end

            STATE_PAUSE: begin
                fifo_wrreq <= 1'b0;
                state <= STATE_READ1;
            end

            STATE_READ1: begin
                fifo_rdreq <= 1'b1;
                state <= STATE_READ2;
            end

            STATE_READ2: begin
                state <= STATE_READ3;
            end

            STATE_READ3: begin
                state <= STATE_HALT;
            end

            STATE_HALT: begin
                fifo_rdreq <= 1'b0;
                $finish;
            end

            default: begin
                state <= STATE_INIT;
            end

        endcase
    end

endmodule
