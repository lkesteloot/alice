
// Streams the contents of memory to an LCD through a FIFO.

module Frame_buffer
    #(parameter ADDRESS=0, LENGTH=0)
(
    input wire clock,
    input wire reset_n,

    // Memory interface:
    output reg [28:0] address,
    output wire [7:0] burstcount,
    input wire waitrequest,
    input wire [63:0] readdata,
    input wire readdatavalid,
    output reg read,
    output wire [63:0] writedata,
    output wire [7:0] byteenable,
    output wire write,

    // LCD interface:
    input wire lcd_tick,
    output wire [7:0] lcd_red,
    output wire [7:0] lcd_green,
    output wire [7:0] lcd_blue,
    input wire lcd_data_enable,

    // These get displayed on the LCD.
    output wire [31:0] debug_value0,
    output wire [31:0] debug_value1,
    output wire [31:0] debug_value2
);

// Convert to 64-bit-based addresses.
localparam FIRST_ADDRESS_64BIT = ADDRESS/8;
localparam LAST_ADDRESS_64BIT = FIRST_ADDRESS_64BIT + LENGTH/8 - 1;

// State machine for transferring data from SDRAM to FIFO.
localparam M2F_STATE_START_FRAME = 4'h00;
localparam M2F_STATE_READ = 4'h01;
localparam M2F_STATE_READ_WAIT = 4'h02;
localparam M2F_STATE_WRITE_WAIT = 4'h03;
reg [3:0] m2f_state;

// State machine for transferring data from FIFO to LCD.
localparam F2L_STATE_IDLE = 4'h00;
localparam F2L_STATE_WAIT = 4'h01;
reg [3:0] f2l_state;

// Registers and assignments.
assign burstcount = 8'h01;
assign byteenable = 8'hFF;
assign writedata = 64'b0;
assign write = 1'b0;
reg [63:0] data;

assign debug_value0 = {
    3'b0, waitrequest,
    3'b0, readdatavalid,
    3'b0, read,
    4'b0,

    3'b0, fifo_write,
    3'b0, fifo_write_wait,
    3'b0, fifo_read_wait,
    m2f_state
};
assign debug_value1 = { 3'b0, address };
assign debug_value2 = { 24'b0, latency_latched };

// FIFO.
reg fifo_write;
wire fifo_write_wait;
wire fifo_read_wait;
reg fifo_read;
wire [63:0] fifo_read_data;
reg [31:0] fifo_read_data_latched;
fb_fifo frame_buffer_fifo(
	.clk_clk(clock),
	.reset_reset_n(reset_n),
	.fifo_0_in_writedata(readdata),
	.fifo_0_in_write(readdatavalid),
	.fifo_0_in_waitrequest(fifo_write_wait),
	.fifo_0_out_readdata(fifo_read_data),
	.fifo_0_out_read(fifo_read),
	.fifo_0_out_waitrequest(fifo_read_wait));

// State machine.
reg [7:0] latency;
reg [7:0] latency_latched;
/*
always @(posedge clock or negedge reset_n) begin
    if (!reset_n) begin
        m2f_state <= M2F_STATE_START_FRAME;
        address <= 29'h0;
        read <= 1'b0;
        fifo_write <= 1'b0;
        latency <= 8'b0;
        latency_latched <= 8'b0;
    end else begin
        case (m2f_state)
            M2F_STATE_START_FRAME: begin
                // Start at beginning of frame memory.
                address <= FIRST_ADDRESS_64BIT;
                read <= 1'b1;
                m2f_state <= M2F_STATE_READ_WAIT;
                latency <= 8'b0;
            end

            M2F_STATE_READ_WAIT: begin
                // When no longer told to wait, de-assert the request lines.
                if (!waitrequest) begin
                    read <= 1'b0;
                end

                // See if the SDRAM got us the data.
                if (readdatavalid) begin
                    // Grab it.
                    data <= readdata;

                    // Next address.
                    if (address == LAST_ADDRESS_64BIT) begin
                        // We're done with this frame.
                        address <= FIRST_ADDRESS_64BIT;
                    end else begin
                        address <= address + 1'b1;
                    end

                    // Write data to the FIFO.
                    fifo_write <= 1'b1;
                    m2f_state <= M2F_STATE_WRITE_WAIT;

                    latency_latched <= latency;
                end else begin
                    latency <= latency + 1'b1;
                end
            end

            M2F_STATE_WRITE_WAIT: begin
                // Wait until we're not requested to wait.
                if (!fifo_write_wait) begin
                    // Stop writing to FIFO.
                    fifo_write <= 1'b0;

                    // We've already incremented the address. Start the
                    // next read.
                    read <= 1'b1;
                    m2f_state <= M2F_STATE_READ_WAIT;
                    latency <= 8'b0;
                end
            end

            default: begin
                // Bug. Just restart.
                m2f_state <= M2F_STATE_START_FRAME;
            end
        endcase
    end
end
*/

reg [28:0] next_address;
always @(posedge clock or negedge reset_n) begin
    if (!reset_n) begin
        m2f_state <= M2F_STATE_START_FRAME;
        address <= 29'h0;
        next_address <= 29'h0;
        read <= 1'b0;
        latency <= 8'b0;
    end else begin
        case (m2f_state)
            M2F_STATE_START_FRAME: begin
                // Start at beginning of frame memory.
                address <= FIRST_ADDRESS_64BIT;
                next_address <= FIRST_ADDRESS_64BIT;
                m2f_state <= M2F_STATE_READ;
            end

            M2F_STATE_READ: begin
                // If we initiated a read already, and the memory controller
                // is busy, we have to wait.
                if (read && waitrequest) begin
                    // Do nothing.
                end else begin
                    // See if there's space in the FIFO. We pipeline our
                    // reads, so we must ensure before the start of the
                    // read that we'll have space once the data comes
                    // back.
                    if (!fifo_write_wait) begin
                        // Initiate a read.
                        read <= 1'b1;
                        address <= next_address;

                        // Compute the next address after this read.
                        if (next_address == LAST_ADDRESS_64BIT) begin
                            // We're done with this frame.
                            next_address <= FIRST_ADDRESS_64BIT;
                        end else begin
                            next_address <= next_address + 1'b1;
                        end
                    end else begin
                        // Not reading this clock cycle.
                        read <= 1'b0;
                    end
                end
            end

            default: begin
                // Bug. Just restart.
                m2f_state <= M2F_STATE_START_FRAME;
            end
        endcase
    end
end

// Get pixel data out of the FIFO.
reg [63:0] pixel_data;
reg need_shifting;
assign lcd_red = pixel_data[7:0];
assign lcd_green = pixel_data[15:8];
assign lcd_blue = pixel_data[23:16];

always @(posedge clock or negedge reset_n) begin
    if (!reset_n) begin
        f2l_state <= F2L_STATE_IDLE;
        fifo_read <= 1'b0;
        need_shifting <= 1'b0;
    end else begin
        case (f2l_state)
            F2L_STATE_IDLE: begin
                // Only do anything when we're on the correct clock for the
                // LCD, otherwise we'll generate pixels too fast.
                if (lcd_tick && lcd_data_enable) begin
                    if (need_shifting) begin
                        // Next pixel.
                        pixel_data[31:0] <= pixel_data[63:32];
                        need_shifting <= 1'b0;
                    end else begin
                        // Start a FIFO read. Data will be available next clock.
                        fifo_read <= 1'b1;
                        f2l_state <= F2L_STATE_WAIT;
                    end
                end
            end

            F2L_STATE_WAIT: begin
                if (fifo_read_wait) begin
                    // We missed our window, the FIFO was empty.
                    pixel_data <= 64'h00ff00ff_00ff00ff; // Magenta.
                end else begin
                    // Grab the data.
                    pixel_data <= fifo_read_data;
                end
                need_shifting <= 1'b1;

                // Either way go back to waiting.
                f2l_state <= F2L_STATE_IDLE;
                fifo_read <= 1'b0;
            end

            default: begin
                // Bug.
                f2l_state <= F2L_STATE_IDLE;
            end
        endcase
    end
end

// Dump the FIFO to the debug output, one per second.
/*
reg [25:0] counter;
always @(posedge clock or negedge reset_n) begin
    if (!reset_n) begin
        counter <= 26'b0;
        fifo_read <= 1'b0;
        fifo_read_data_latched <= 32'hDEAD_BEEF;
    end else begin
        if (counter == 26'd49_999_999) begin
            fifo_read <= 1'b1;
            counter <= counter + 1'b1;
        end else if (counter == 26'd50_000_000) begin
            if (!fifo_read_wait) begin
                fifo_read <= 1'b0;
                fifo_read_data_latched <= fifo_read_data[31:0];
                counter <= 0;
            end
        end else begin
            counter <= counter + 1'b1;
        end
    end
end
*/

endmodule
