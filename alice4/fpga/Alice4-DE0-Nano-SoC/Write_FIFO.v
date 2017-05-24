// Queues up writes (color and Z) to the frame buffer.
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
    output wire [FIFO_DEPTH_LOG2-1:0] size
);

    // Constants.
    assign write_color_burstcount = 8'h01;
    assign write_z_burstcount = 8'h01;

    // State machine.
    localparam STATE_INIT = 2'h0;
    localparam STATE_WRITE = 2'h1;
    localparam STATE_WAIT = 2'h2;
    reg [1:0] state;

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
    /// reg [187:0] fifo_read_data_reg;
    /// reg [187:0] fifo_read_data_reg_tmp;
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

            // State machine.
            state <= STATE_INIT;
        end else begin
            case (state)
                STATE_INIT: begin
                    if (!fifo_empty) begin
                        fifo_read <= 1'b1;
                        state <= STATE_WRITE;
                    end
                end

                STATE_WRITE: begin
                    fifo_read <= 1'b0;

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
        end

            /*
            if (write_color_write && !write_color_waitrequest) begin
                write_color_write <= 1'b0;
            end

            if (write_z_write && !write_z_waitrequest) begin
                write_z_write <= 1'b0;
            end

            // See if we're writing and should hold off.
            if (write_color_write && write_color_waitrequest) begin
                // Waiting for color.
                if (write_z_write && write_z_waitrequest) begin
                    // Waiting for Z. Nothing to do.
                end else begin
                    // Done with Z, de-assert write.
                    write_z_write <= 1'b0;
                end
            end else begin
                if (write_z_write && write_z_waitrequest) begin
            end

            if ((!write_color_write || !write_color_waitrequest) &&
                (!write_z_write || !write_z_waitrequest) &&
                !fifo_empty) begin

                // Initiate a read from the FIFO. The data will be available
                // on the next clock.
                fifo_read <= 1'b1;
            end

            // See if we requested a read last clock.
            if (fifo_read) begin
                // See if the memory controller is still busy with our
                // previous write.
                if ((!write_color_write || !write_color_waitrequest) &&
                    (!write_z_write || !write_z_waitrequest)) begin

                    // Not busy. Can write. Grab the data.
                    fifo_read_data_reg <= fifo_read_data;

                    // Also set the various memory write parameters.
                    write_color_address <= fifo_color_address;
                    write_color_writedata <= fifo_color;
                    write_color_byteenable <= fifo_byte_enable;
                    write_color_write <= 1'b1;
                    write_z_address <= fifo_z_address;
                    write_z_writedata <= fifo_z;
                    write_z_byteenable <= fifo_byte_enable;
                    write_z_write <= 1'b1; // TODO add bit in queue for whether to do this.
                end else begin
                    // Still busy. Grab the data, but put it in a temporary
                    // register.
                    fifo_read_data_reg_tmp <= fifo_read_data;
                end

            end
        end
        */
    end

endmodule
