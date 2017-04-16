
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
    input wire lcd_next_frame,
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
localparam M2F_STATE_IDLE = 4'h01;
localparam M2F_STATE_READ = 4'h02;
localparam M2F_STATE_READ_WAIT = 4'h03;
localparam M2F_STATE_WRITE_WAIT = 4'h04;
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
assign debug_value1 = word_count_latched;
assign debug_value2 = pixel_count_latched;

// FIFO.
localparam FIFO_DEPTH = 32;
localparam FIFO_DEPTH_LOG2 = 5;
localparam BURST_LENGTH = FIFO_DEPTH/2;
reg fifo_write;
wire fifo_write_wait;
wire fifo_read_wait;
reg fifo_read;
wire [63:0] fifo_read_data;
reg [31:0] fifo_read_data_latched;
wire [FIFO_DEPTH_LOG2 - 1:0] fifo_usedw;
/*
fb_fifo frame_buffer_fifo(
	.clk_clk(clock),
	.reset_reset_n(reset_n),
	.fifo_0_in_writedata(readdata),
	.fifo_0_in_write(readdatavalid),
	.fifo_0_in_waitrequest(fifo_write_wait),
	.fifo_0_out_readdata(fifo_read_data),
	.fifo_0_out_read(fifo_read),
	.fifo_0_out_waitrequest(fifo_read_wait));
*/
scfifo frame_buffer_fifo(
        .aclr(!reset_n),
        .clock(clock),
        .data(readdata),
        .empty(fifo_read_wait),
        .full(fifo_write_wait),
        .usedw(fifo_usedw),
        .q(fifo_read_data),
        .rdreq(fifo_read),
        .wrreq(readdatavalid && !fifo_write_wait));
defparam frame_buffer_fifo.add_ram_output_register = "OFF",
         frame_buffer_fifo.intended_device_family = "CYCLONEV",
         frame_buffer_fifo.lpm_numwords = FIFO_DEPTH,
         frame_buffer_fifo.lpm_showahead = "OFF",
         frame_buffer_fifo.lpm_type = "scfifo",
         frame_buffer_fifo.lpm_width = 64,
         frame_buffer_fifo.lpm_widthu = FIFO_DEPTH_LOG2,
         frame_buffer_fifo.overflow_checking = "ON",
         frame_buffer_fifo.underflow_checking = "ON",
         frame_buffer_fifo.use_eab = "ON";

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

// The next address to read after this one.
reg [28:0] next_address;
// For debugging.
reg [25:0] word_count;
reg [25:0] word_count_latched;
// Keep track of bursts.
reg [5:0] words_requested;
reg [5:0] words_read;
always @(posedge clock or negedge reset_n) begin
    if (!reset_n) begin
        m2f_state <= M2F_STATE_START_FRAME;
        address <= 29'h0;
        next_address <= 29'h0;
        read <= 1'b0;
        latency <= 8'b0;
        word_count <= 1'b0;
        word_count_latched <= 1'b0;
        words_requested <= 1'b0;
        words_read <= 1'b0;
    end else begin
        case (m2f_state)
            // Initial state.
            M2F_STATE_START_FRAME: begin
                // Start at beginning of frame memory.
                address <= FIRST_ADDRESS_64BIT;
                next_address <= FIRST_ADDRESS_64BIT;
                m2f_state <= M2F_STATE_IDLE;
            end

            // Wait for FIFO to be half-empty.
            M2F_STATE_IDLE: begin
                if (!fifo_usedw[FIFO_DEPTH_LOG2 - 1]) begin
                    // Start burst reading.
                    words_requested <= 1'b0;
                    words_read <= 1'b0;
                    m2f_state <= M2F_STATE_READ;
                end
            end

            // Initiate BURST_LENGTH reads and wait for them to complete.
            M2F_STATE_READ: begin
                // If we initiated a read already, and the memory controller
                // is busy, we have to wait.
                if (read && waitrequest) begin
                    // Do nothing.
                end else begin
                    // Keep reading a whole burst length.
                    if (words_requested < BURST_LENGTH) begin
                        // Initiate a read.
                        read <= 1'b1;
                        address <= next_address;
                        words_requested <= words_requested + 1'b1;

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

                // If a word has returned from memory, keep track of it.
                // We don't want to exit this state (and risk re-entering
                // it) until all words have come back.
                if (readdatavalid) begin
                    if (words_read == BURST_LENGTH - 1) begin
                        // Got all the words from this burst.
                        m2f_state <= M2F_STATE_IDLE;
                    end else begin
                        words_read <= words_read + 1'b1;
                    end
                end
            end

            default: begin
                // Bug. Just restart.
                m2f_state <= M2F_STATE_START_FRAME;
            end
        endcase

        if (readdatavalid) begin
            word_count <= word_count + 1'b1;
        end

        if (lcd_next_frame) begin
            if (word_count >= 128) begin
                word_count_latched <= word_count;
            end
            word_count <= 1'b0;
        end
    end
end

// Get pixel data out of the FIFO.
reg [63:0] pixel_data;
reg need_shifting;
assign lcd_red = pixel_data[7:0];
assign lcd_green = pixel_data[15:8];
assign lcd_blue = pixel_data[23:16];
reg [25:0] pixel_count;
reg [25:0] pixel_count_latched;

always @(posedge clock or negedge reset_n) begin
    if (!reset_n) begin
        f2l_state <= F2L_STATE_IDLE;
        fifo_read <= 1'b0;
        need_shifting <= 1'b0;
        pixel_count <= 1'b0;
        pixel_count_latched <= 1'b0;
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

        if (fifo_read) begin
            pixel_count <= pixel_count + 1'b1;
        end

        if (lcd_next_frame) begin
            if (pixel_count >= 128) begin
                pixel_count_latched <= pixel_count;
            end
            pixel_count <= 1'b0;
        end
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
