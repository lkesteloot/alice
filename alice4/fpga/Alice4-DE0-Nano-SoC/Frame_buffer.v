
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

    // LCD interface:
    input wire lcd_tick,
    input wire lcd_next_frame,
    output wire [7:0] lcd_red,
    output wire [7:0] lcd_green,
    output wire [7:0] lcd_blue,
    input wire lcd_data_enable,
    output reg lcd_data_enable_delayed,

    // Front buffer handling.
    input wire rast_front_buffer,
    output reg fb_front_buffer,

    // These get displayed on the LCD.
    output wire [31:0] debug_value0,
    output wire [31:0] debug_value1,
    output wire [31:0] debug_value2
);

// Convert to 64-bit-based addresses.
/* verilator lint_off WIDTH */
localparam [28:0] FIRST_ADDRESS0_64BIT = ADDRESS/8;
localparam [28:0] LAST_ADDRESS0_64BIT = FIRST_ADDRESS0_64BIT + LENGTH/8 - 1;
localparam [28:0] FIRST_ADDRESS1_64BIT = ADDRESS/8 + LENGTH/8;
localparam [28:0] LAST_ADDRESS1_64BIT = FIRST_ADDRESS1_64BIT + LENGTH/8 - 1;
/* verilator lint_on WIDTH */

// State machine for transferring data from SDRAM to FIFO.
localparam M2F_STATE_START_FRAME = 4'h0;
localparam M2F_STATE_IDLE = 4'h1;
localparam M2F_STATE_READ = 4'h2;
localparam M2F_STATE_FLUSH_SDRAM = 4'h5;
localparam M2F_STATE_FLUSH_FIFO = 4'h6;
reg [3:0] m2f_state /* verilator public */;

// State machine for transferring data from FIFO to LCD.
localparam F2L_STATE_IDLE = 2'h0;
localparam F2L_STATE_WAIT = 2'h1;
localparam F2L_STATE_READ = 2'h2;
reg [1:0] f2l_state;

// Registers and assignments.
assign burstcount = 8'h01;

assign debug_value0 = {
    3'b0, waitrequest,
    3'b0, readdatavalid,
    3'b0, read,
    4'b0,

    4'b0,
    3'b0, fifo_write_wait,
    3'b0, fifo_read_wait,
    m2f_state
};
assign debug_value1 = { 6'b0, word_count_latched };
assign debug_value2 = { 6'b0, pixel_count_latched };

// FIFO.
localparam FIFO_DEPTH = 256;
localparam FIFO_DEPTH_LOG2 = 8;
localparam BURST_LENGTH = 8; // XYZ 32;
reg fifo_sclr;
wire fifo_write_wait;
wire fifo_read_wait;
wire fifo_write /* verilator public */;
reg fifo_read /* verilator public */;
wire [63:0] fifo_read_data /* verilator public */;
wire [FIFO_DEPTH_LOG2 - 1:0] fifo_usedw /* verilator public */;
assign fifo_write = readdatavalid /*&& !fifo_write_wait*/; // XXX suspicious, fifo_write_wait not valid initially?
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
         .use_eab("ON")) frame_buffer_fifo(
        .aclr(!reset_n),
        .sclr(fifo_sclr),
        .clock(clock),
        .data(readdata),
        .empty(fifo_read_wait),
        .full(fifo_write_wait),
        .usedw(fifo_usedw),
        .q(fifo_read_data),
        .rdreq(fifo_read),
        .wrreq(fifo_write));
/* verilator lint_on PINMISSING */

// The next address to read after this one.
reg [28:0] next_address;
// For debugging.
reg [25:0] word_count;
reg [25:0] word_count_latched;
// Keep track of bursts.
reg [5:0] words_requested /* verilator public */;
reg [5:0] words_read /* verilator public */;
// State machine.
always @(posedge clock or negedge reset_n) begin
    if (!reset_n) begin
        m2f_state <= M2F_STATE_START_FRAME;
        address <= 29'h0;
        next_address <= 29'h0;
        read <= 1'b0;
        fb_front_buffer <= 1'b0;
        word_count <= 26'b0;
        word_count_latched <= 26'b0;
        words_requested <= 6'b0;
        words_read <= 6'b0;
        fifo_sclr <= 1'b0;
    end else begin
        case (m2f_state)
            // Initial state.
            M2F_STATE_START_FRAME: begin
                fb_front_buffer <= rast_front_buffer;

                // Start at beginning of front buffer frame memory.
                if (rast_front_buffer) begin
                    next_address <= FIRST_ADDRESS1_64BIT;
                end else begin
                    next_address <= FIRST_ADDRESS0_64BIT;
                end
                fifo_sclr <= 1'b0;
                m2f_state <= M2F_STATE_IDLE;
            end

            // Wait for FIFO to have space (one burst and then some).
            M2F_STATE_IDLE: begin
                // If we should start the next frame, abort and
                // flush the queue.
                if (lcd_next_frame) begin
                    m2f_state <= M2F_STATE_FLUSH_SDRAM;
                end else if ({ 1'b0, fifo_usedw } < FIFO_DEPTH - BURST_LENGTH - 8) begin
                    // Start burst reading.
                    words_requested <= 6'b0;
                    words_read <= 6'b0;
                    m2f_state <= M2F_STATE_READ;
                end
            end

            // Initiate BURST_LENGTH reads and wait for them to complete.
            M2F_STATE_READ: begin
                // If we should start the next frame, abort and
                // flush the queue.
                if (lcd_next_frame) begin
                    read <= 1'b0;
                    m2f_state <= M2F_STATE_FLUSH_SDRAM;
                end else if (read && waitrequest) begin
                    // If we initiated a read already, and the memory
                    // controller is busy, we have to wait.
                end else begin
                    // Keep reading a whole burst length.
                    if (words_requested < BURST_LENGTH) begin
                        // Initiate a read.
                        read <= 1'b1;
                        address <= next_address;
                        words_requested <= words_requested + 1'b1;

                        // Compute the next address after this read.
                        if (next_address == LAST_ADDRESS0_64BIT ||
                            next_address == LAST_ADDRESS1_64BIT) begin

                            // We're done with this frame.
                            if (fb_front_buffer) begin
                                next_address <= FIRST_ADDRESS1_64BIT;
                            end else begin
                                next_address <= FIRST_ADDRESS0_64BIT;
                            end
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
                    if (!lcd_next_frame && words_read == BURST_LENGTH - 1) begin
                        // Got all the words from this burst.
                        m2f_state <= M2F_STATE_IDLE;
                    end
                    words_read <= words_read + 1'b1;
                end
            end

            // Flush the SDRAM queue.
            M2F_STATE_FLUSH_SDRAM: begin
                // We've perhaps queued up a bunch of reads,
                // because reads can be pipelined. We need for
                // all those reads to finish before we can
                // flush the FIFO.
                if (readdatavalid) begin
                    // We've read another word.
                    words_read <= words_read + 1'b1;
                end
                if (words_requested == words_read) begin
                    // Done flushing the SDRAM. Now we can flush the FIFO.
                    m2f_state <= M2F_STATE_FLUSH_FIFO;
                end
            end

            // Flush the FIFO.
            M2F_STATE_FLUSH_FIFO: begin
                fifo_sclr <= 1'b1;
                m2f_state <= M2F_STATE_START_FRAME;
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
            word_count <= 26'b0;
        end
    end
end

// Get pixel data out of the FIFO.
/* verilator lint_off UNUSED */
reg [63:0] pixel_data;
/* verilator lint_on UNUSED */
reg need_shifting;
assign lcd_red = pixel_data[7:0];
assign lcd_green = pixel_data[15:8];
assign lcd_blue = pixel_data[23:16];
reg [25:0] pixel_count;
reg [25:0] pixel_count_latched;

// For delaying the data enable signal.
reg lcd_data_enable_d1;
reg lcd_data_enable_d2;

always @(posedge clock or negedge reset_n) begin
    if (!reset_n) begin
        f2l_state <= F2L_STATE_IDLE;
        fifo_read <= 1'b0;
        need_shifting <= 1'b0;
        pixel_count <= 26'b0;
        pixel_count_latched <= 26'b0;
        lcd_data_enable_d1 <= 1'b0;
        lcd_data_enable_d2 <= 1'b0;
    end else begin
        // If we're clearing the FIFO, clear everything else too.
        if (fifo_sclr) begin
            f2l_state <= F2L_STATE_IDLE;
            fifo_read <= 1'b0;
            need_shifting <= 1'b0;

            // Debug output.
            if (pixel_count >= 128) begin
                pixel_count_latched <= pixel_count;
            end
            pixel_count <= 26'b0;
        end else begin
            // Delay data enable by three clocks, since that's how long it
            // takes for us to realize we need to pull from the FIFO and wait
            // for the FIFO data.
            lcd_data_enable_d1 <= lcd_data_enable;
            lcd_data_enable_d2 <= lcd_data_enable_d1;
            lcd_data_enable_delayed <= lcd_data_enable_d2;

            case (f2l_state)
                F2L_STATE_IDLE: begin
                    // See if we should pre-fetch the head of the FIFO.
                    if (!lcd_tick && !need_shifting && lcd_data_enable) begin
                        // Start a FIFO read. Data will be available in
                        // two clocks.
                        fifo_read <= 1'b1;
                        f2l_state <= F2L_STATE_WAIT;
                    end else begin
                        // See if we need to shift the pixel from
                        // the high 32 bits to the low ones.
                        if (lcd_tick && lcd_data_enable && need_shifting) begin
                            // Next pixel.
                            $display("--------------------- shifting pixel_data");
                            pixel_data[31:0] <= pixel_data[63:32];
                            need_shifting <= 1'b0;
                        end
                    end
                end

                F2L_STATE_WAIT: begin
                    // We can check this one clock after we request a read.
                    if (fifo_read_wait) begin
                        // We missed it. Nothing we can do here will
                        // fix it, so draw some magenta so that it's
                        // obvious something went wrong.
                        pixel_data <= 64'h00ff00ff_00ff00ff;
                        f2l_state <= F2L_STATE_IDLE;
                    end else begin
                        // Must wait one more clock to get the data.
                        f2l_state <= F2L_STATE_READ;
                    end

                    // Either way stop reading.
                    fifo_read <= 1'b0;
                end

                F2L_STATE_READ: begin
                    $display("--------------------- reading into pixel_data");
                    // Grab the data from the FIFO.
                    pixel_data <= fifo_read_data;

                    // We got a 64-bit number, but our pixels are
                    // 32 bits. Record that fact that we still need
                    // to shift the high 32 bits into the lower ones.
                    need_shifting <= 1'b1;

                    // Either way go back to waiting.
                    f2l_state <= F2L_STATE_IDLE;
                end

                default: begin
                    // Bug.
                    f2l_state <= F2L_STATE_IDLE;
                end
            endcase

            // Debug output.
            if (fifo_read) begin
                pixel_count <= pixel_count + 1'b1;
            end
        end
    end
end

endmodule
