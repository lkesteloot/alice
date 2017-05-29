// Queues up writes (color and Z) to the frame buffer.

`define PIPELINED

module Write_FIFO
    #(parameter FIFO_DEPTH=32, FIFO_DEPTH_LOG2=5)
(
    // Clock and reset.
    input wire clock,
    input wire reset_n,

    // Memory interface for writing color pixels.
    output reg [28:0] write_color_address,
    output wire [7:0] write_color_burstcount,
    input wire write_color_waitrequest,
    output reg [63:0] write_color_writedata,
    output reg [7:0] write_color_byteenable,
    output reg write_color_write,

    // Memory interface for writing Z pixels.
    output reg [28:0] write_z_address,
    output wire [7:0] write_z_burstcount,
    input wire write_z_waitrequest,
    output reg [63:0] write_z_writedata,
    output reg [7:0] write_z_byteenable,
    output reg write_z_write,

    // FIFO controls.
    input wire enqueue,
    input wire [28:0] color_address,
    input wire [63:0] color,
    input wire [28:0] z_address,
    input wire [63:0] z,
    input wire [1:0] pixel_active, // Bit 0 is the left-most pixel.
    output wire [FIFO_DEPTH_LOG2-1:0] size,

    // Indicates an internal error that should be displayed to the user during
    // debugging.
    output reg [7:0] error
);

    // Constants.
    assign write_color_burstcount = 8'h01;
    assign write_z_burstcount = 8'h01;

`ifdef PIPELINED
    // State.
    reg [187:0] slot1;
    wire [28:0] slot1_color_address = slot1[28:0];
    wire [63:0] slot1_color = slot1[92:29];
    wire [28:0] slot1_z_address = slot1[121:93];
    wire [63:0] slot1_z = slot1[185:122];
    wire [1:0] slot1_pixel_active = slot1[187:186];
    wire [7:0] slot1_byte_enable = {{4{slot1_pixel_active[1]}}, {4{slot1_pixel_active[0]}}};
    reg slot1_full;
    reg [187:0] slot2;
    reg slot2_full;
    // According to the Avalon-MM spec, the waitrequest lines should only be
    // inspected when we're trying to read or write. They can be any value at
    // other times. So we make these signals to take that into account.
    wire color_wait = write_color_write && write_color_waitrequest;
    wire z_wait = write_z_write && write_z_waitrequest;
    // Keep track of whether there was data in the queue when we read it.
    reg got_queue_data;
`else
    // State machine.
    localparam STATE_INIT = 2'h0;
    localparam STATE_READ = 2'h1;
    localparam STATE_WRITE = 2'h2;
    localparam STATE_WAIT = 2'h3;
    reg [1:0] state;
`endif

    // Pack the FIFO data.
    wire [187:0] fifo_write_data = {
        pixel_active,
        z,
        z_address,
        color,
        color_address
    };
    // Unpack the FIFO data.
    wire [187:0] fifo_read_data;
    wire [28:0] fifo_color_address = fifo_read_data[28:0];
    wire [63:0] fifo_color = fifo_read_data[92:29];
    wire [28:0] fifo_z_address = fifo_read_data[121:93];
    wire [63:0] fifo_z = fifo_read_data[185:122];
    wire [1:0] fifo_pixel_active = fifo_read_data[187:186];
    wire [7:0] fifo_byte_enable = {{4{fifo_pixel_active[1]}}, {4{fifo_pixel_active[0]}}};

    // FIFO implementation.
    wire fifo_empty;
    wire fifo_full;
    reg fifo_read;
    scfifo fifo(
            .aclr(!reset_n),
            .clock(clock),
            .data(fifo_write_data),
            .empty(fifo_empty),
            .full(fifo_full),
            .usedw(size),
            .q(fifo_read_data),
            .rdreq(fifo_read),
            .wrreq(enqueue));
    defparam fifo.add_ram_output_register = "OFF",
             fifo.intended_device_family = "CYCLONEV",
             fifo.lpm_numwords = FIFO_DEPTH,
             fifo.lpm_showahead = "OFF",
             fifo.lpm_type = "scfifo",
             fifo.lpm_width = 188,
             fifo.lpm_widthu = FIFO_DEPTH_LOG2,
             fifo.overflow_checking = "ON",
             fifo.underflow_checking = "ON",
             fifo.use_eab = "ON";

    // Dequeuing.
    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            // Memory.
            write_color_address <= 1'b0;
            write_color_writedata <= 1'b0;
            write_color_byteenable <= 1'b0;
            write_color_write <= 1'b0;
            write_z_address <= 1'b0;
            write_z_writedata <= 1'b0;
            write_z_byteenable <= 1'b0;
            write_z_write <= 1'b0;

            // FIFO.
            fifo_read <= 1'b0;

`ifdef PIPELINED
            // State.
            slot1 <= 1'b0;
            slot1_full <= 1'b0;
            slot2 <= 1'b0;
            slot2_full <= 1'b0;
            error <= 1'b0;
            got_queue_data <= 1'b0;
`else
            // State machine.
            state <= STATE_INIT;
`endif
        end else begin
`ifdef PIPELINED
            // Only read from the queue if we're not asked to wait for memory,
            // and we have no slots with data. Since it takes a few clock
            // cycles for us to realize that we should wait, we need some
            // slots to dump the data into in the meantime.
            fifo_read <= !color_wait && !z_wait && !slot1_full;

            // One clock after fifo_read, the empty signal indicates whether the queue
            // was empty when we tried to read it.
            got_queue_data <= fifo_read && !fifo_empty;

            // Default to no error unless something happens below.
            error <= 1'b0;

            // Two clocks after fifo_read, the data is ready. Pack everything
            // we know into a single value we can switch on.
            casez ({got_queue_data, color_wait, z_wait, slot1_full, slot2_full})
                5'b0_00_00: begin
                    // No new data, not waiting, no old data. Nothing to do.
                    write_color_write <= 1'b0;
                    write_z_write <= 1'b0;
                end

                5'b?_??_01: begin
                    // Can't have slot2 filled without slot1 filled; raise an error.
                    error <= 8'h80;
                end

                5'b0_00_10: begin
                    // No new data, but we have old data that can be written.
                    write_color_address <= slot1_color_address;
                    write_color_writedata <= slot1_color;
                    write_color_byteenable <= slot1_byte_enable;
                    write_color_write <= slot1_color_address != 1'b0;
                    write_z_address <= slot1_z_address;
                    write_z_writedata <= slot1_z;
                    write_z_byteenable <= slot1_byte_enable;
                    write_z_write <= slot1_z_address != 1'b0;
                    slot1_full <= 1'b0;
                end

                5'b0_00_11: begin
                    // No new data, but we have old data that can be written.
                    write_color_address <= slot1_color_address;
                    write_color_writedata <= slot1_color;
                    write_color_byteenable <= slot1_byte_enable;
                    write_color_write <= slot1_color_address != 1'b0;
                    write_z_address <= slot1_z_address;
                    write_z_writedata <= slot1_z;
                    write_z_byteenable <= slot1_byte_enable;
                    write_z_write <= slot1_z_address != 1'b0;
                    slot1 <= slot2;
                    slot2_full <= 1'b0;
                end

                5'b0_11_00, 5'b0_01_00, 5'b0_10_00,
                5'b0_11_10, 5'b0_01_10, 5'b0_10_10,
                5'b0_11_11, 5'b0_01_11, 5'b0_10_11: begin
                    // Waiting for memory but no new data. Doesn't matter
                    // what's in the slots, they'll wait.
                    write_color_write <= color_wait;
                    write_z_write <= z_wait;
                end

                5'b1_00_00: begin
                    // Got new data, can use it right away.
                    write_color_address <= fifo_color_address;
                    write_color_writedata <= fifo_color;
                    write_color_byteenable <= fifo_byte_enable;
                    write_color_write <= fifo_color_address != 1'b0;
                    write_z_address <= fifo_z_address;
                    write_z_writedata <= fifo_z;
                    write_z_byteenable <= fifo_byte_enable;
                    write_z_write <= fifo_z_address != 1'b0;
                end

                5'b1_00_10: begin
                    // Got new data, replace slot1 and send slot1 to memory.
                    write_color_address <= slot1_color_address;
                    write_color_writedata <= slot1_color;
                    write_color_byteenable <= slot1_byte_enable;
                    write_color_write <= slot1_color_address != 1'b0;
                    write_z_address <= slot1_z_address;
                    write_z_writedata <= slot1_z;
                    write_z_byteenable <= slot1_byte_enable;
                    write_z_write <= slot1_z_address != 1'b0;
                    slot1 <= fifo_read_data;
                end

                5'b1_00_11: begin
                    // Got new data, replace slot2, slot2 to slot1, and send
                    // slot1 to memory.
                    write_color_address <= slot1_color_address;
                    write_color_writedata <= slot1_color;
                    write_color_byteenable <= slot1_byte_enable;
                    write_color_write <= slot1_color_address != 1'b0;
                    write_z_address <= slot1_z_address;
                    write_z_writedata <= slot1_z;
                    write_z_byteenable <= slot1_byte_enable;
                    write_z_write <= slot1_z_address != 1'b0;
                    slot1 <= slot2;
                    slot2 <= fifo_read_data;
                end

                5'b1_11_00, 5'b1_01_00, 5'b1_10_00: begin
                    // Have new data but can't write to memory. Put it in slot1.
                    slot1 <= fifo_read_data;
                    slot1_full <= 1'b1;
                    write_color_write <= color_wait;
                    write_z_write <= z_wait;
                end

                5'b1_11_10, 5'b1_01_10, 5'b1_10_10: begin
                    // Have new data but can't write to memory and slot1 is
                    // full. Put it in slot2.
                    slot2 <= fifo_read_data;
                    slot2_full <= 1'b1;
                    write_color_write <= color_wait;
                    write_z_write <= z_wait;
                end

                5'b1_11_11, 5'b1_01_11, 5'b1_10_11: begin
                    // In principle this can't happen because we wouldn't have asked to
                    // read from the queue. Raise an error.
                    error <= 8'h40 | {got_queue_data, color_wait, z_wait, slot1_full, slot2_full};
                end

                default: begin
                    // I think I covered all the cases, but put this here just
                    // in case.
                    error <= {got_queue_data, color_wait, z_wait, slot1_full, slot2_full};
                end
            endcase
`else
            case (state)
                STATE_INIT: begin
                    if (!fifo_empty) begin
                        fifo_read <= 1'b1;
                        state <= STATE_READ;
                    end
                end

                STATE_READ: begin
                    fifo_read <= 1'b0;
                    state <= STATE_WRITE;
                end

                STATE_WRITE: begin
                    // Grab the data.
                    /// fifo_read_data_reg <= fifo_read_data;

                    // Also set the various memory write parameters.
                    write_color_address <= fifo_color_address;
                    write_color_writedata <= fifo_color;
                    write_color_byteenable <= fifo_byte_enable;
                    write_color_write <= fifo_color_address != 1'b0;
                    write_z_address <= fifo_z_address;
                    write_z_writedata <= fifo_z;
                    write_z_byteenable <= fifo_byte_enable;
                    write_z_write <= fifo_z_address != 1'b0;

                    state <= STATE_WAIT;
                end

                STATE_WAIT: begin
                    if (write_color_write && !write_color_waitrequest) begin
                        write_color_write <= 1'b0;
                    end

                    if (write_z_write && !write_z_waitrequest) begin
                        write_z_write <= 1'b0;
                    end

                    if ((!write_color_write || !write_color_waitrequest) &&
                        (!write_z_write || !write_z_waitrequest)) begin

                        // All done.
                        state <= STATE_INIT;
                    end
                end

                default: begin
                    state <= STATE_INIT;
                end
            endcase
`endif
        end
    end

endmodule
