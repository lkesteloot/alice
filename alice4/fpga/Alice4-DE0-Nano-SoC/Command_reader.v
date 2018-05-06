
// Reads commands from memory and enqueues them into a FIFO.
module Command_reader
    // Address is in bytes.
    #(parameter CMD_ADDRESS=0, FIFO_DEPTH=32, FIFO_DEPTH_LOG2=5)
(
    // Control.
    input wire clock,
    input wire reset_n,
    input wire restart,
    // No latency after a restart until ready is updated.
    output wire ready,

    // Memory interface for reading command buffer.
    output reg [28:0] read_address,
    output wire [7:0] read_burstcount,
    input wire read_waitrequest,
    input wire [63:0] read_readdata,
    input wire read_readdatavalid,
    output reg read_read,

    // FIFO interface.
    output wire fifo_empty,
    output wire [63:0] fifo_q,
    input wire fifo_rdreq
);

    // Constants.
    assign read_burstcount = 8'h01;

    // State machine.
    localparam STATE_INIT = 3'h0;
    localparam STATE_RESTART = 3'h1;
    localparam STATE_FLUSHING_READS = 3'h2;
    localparam STATE_CLEAR_FIFO_WAIT = 3'h3;
    localparam STATE_COPY_COMMANDS = 3'h4;
    reg [2:0] state;

    // Internal state.
    reg [28:0] pc;
    reg [FIFO_DEPTH-1:0] pending_reads;

    // External state.
    assign ready = state == STATE_COPY_COMMANDS && !restart;

    // FIFO.
    reg fifo_sclr;
    wire [FIFO_DEPTH_LOG2-1:0] fifo_size;
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
            .aclr(!reset_n),
            .sclr(fifo_sclr),
            .clock(clock),
            .data(read_readdata),
            .empty(fifo_empty),
            .usedw(fifo_size),
            .q(fifo_q),
            .rdreq(fifo_rdreq),
            .wrreq(read_readdatavalid));
/* verilator lint_on PINMISSING */

    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            state <= STATE_INIT;
/* verilator lint_off WIDTH */
            pc <= CMD_ADDRESS/8;
/* verilator lint_on WIDTH */
            pending_reads <= {FIFO_DEPTH{1'b0}};
            read_read <= 1'b0;
            fifo_sclr <= 1'b0;
        end else begin
            // Keep track of read responses.
            if (read_readdatavalid) begin
                // We might override this lower down, but this is
                // the default behavior.
                pending_reads <= pending_reads - 1'b1;
            end 

            if (restart) begin
                state <= STATE_RESTART;
                read_read <= 1'b0;
            end else case (state)
                STATE_INIT: begin
                    // Nothing, just wait until we're restarted. We don't
                    // want to do any memory reads until the memory
                    // controller has been fully configured.
                end

                // Restart the module.
                STATE_RESTART: begin
/* verilator lint_off WIDTH */
                    pc <= CMD_ADDRESS/8;
/* verilator lint_on WIDTH */
                    state <= STATE_FLUSHING_READS;
                end

                // Wait for pending reads to flush.
                STATE_FLUSHING_READS: begin
                    if (pending_reads == 0) begin
                        // Now that all reads have been enqueued, we
                        // can clear the FIFO.
                        fifo_sclr <= 1'b1;
                        state <= STATE_CLEAR_FIFO_WAIT;
                    end
                end

                STATE_CLEAR_FIFO_WAIT: begin
                    // Wait one clock for the sync clear to take effect.
                    fifo_sclr <= 1'b0;
                    state <= STATE_COPY_COMMANDS;
                end

                STATE_COPY_COMMANDS: begin
                    // If we're being told to hold our request, do so.
                    if (read_read && read_waitrequest) begin
                        // Do nothing.
/* verilator lint_off WIDTH */
                    end else if (fifo_size + pending_reads < FIFO_DEPTH - 3) begin
/* verilator lint_on WIDTH */
                        // Initiate read from memory.
                        read_address <= pc;
                        read_read <= 1'b1;
                        pc <= pc + 1'b1;

                        if (read_readdatavalid) begin
                            // Override subtraction above.
                            pending_reads <= pending_reads;
                        end else begin
                            pending_reads <= pending_reads + 1'b1;
                        end
                    end else begin
                        // Not reading and can't fit any more into FIFO.
                        read_read <= 1'b0;
                    end
                end

                default: begin
                    // Bug, restart.
                    state <= STATE_INIT;
                end
            endcase
        end
    end

endmodule
