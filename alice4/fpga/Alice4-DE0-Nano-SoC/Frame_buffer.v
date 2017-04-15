
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

// State machine.
localparam STATE_START_FRAME = 4'h00;
localparam STATE_READ_WAIT = 4'h01;
localparam STATE_WRITE_WAIT = 4'h02;
reg [3:0] state;

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
    state
};
assign debug_value1 = { 3'b0, address };
assign debug_value2 = fifo_read_data_latched;

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
	.fifo_0_in_writedata(data),
	.fifo_0_in_write(fifo_write),
	.fifo_0_in_waitrequest(fifo_write_wait),
	.fifo_0_out_readdata(fifo_read_data),
	.fifo_0_out_read(fifo_read),
	.fifo_0_out_waitrequest(fifo_read_wait));

// State machine.
always @(posedge clock or negedge reset_n) begin
    if (!reset_n) begin
        state <= STATE_START_FRAME;
        address <= 29'h0;
        read <= 1'b0;
        fifo_write <= 1'b0;
    end else begin
        case (state)
            STATE_START_FRAME: begin
                // Start at beginning of frame memory.
                address <= FIRST_ADDRESS_64BIT;
                read <= 1'b1;
                state <= STATE_READ_WAIT;
            end

            STATE_READ_WAIT: begin
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
                    state <= STATE_WRITE_WAIT;
                end
            end

            STATE_WRITE_WAIT: begin
                // Wait until we're not requested to wait.
                if (!fifo_write_wait) begin
                    // Stop writing to FIFO.
                    fifo_write <= 1'b0;

                    // We've already incremented the address. Start the
                    // next read.
                    read <= 1'b1;
                    state <= STATE_READ_WAIT;
                end
            end

            default: begin
                // Bug. Just restart.
                state <= STATE_START_FRAME;
            end
        endcase
    end
end

// Get pixel data out of the FIFO.
/*
reg [63:0] this_pixel;
reg [63:0] next_pixel;
assign lcd_red = this_pixel[7:0];
assign lcd_green = this_pixel[15:8];
assign lcd_blue = this_pixel[23:16];

always @(posedge clock or negedge reset_n) begin
    // Only do anything when we're on the correct clock for the LCD,
    // otherwise we'll generate pixels too fast.
    if (lcd_tick && lcd_data_enable) begin
        // Swap to the next pixel and start reading the subsequent one.
        this_pixel <= next_pixel;


	.fifo_0_out_readdata,
	.fifo_0_out_read,
	.fifo_0_out_waitrequest);
    end
end
*/

// Dump the FIFO to the debug output, one per second.
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

endmodule
