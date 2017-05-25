
// Enqueues pixel information and when the Z read
// returns, does a compare and then sends the pixel on to the write FIFO.
module Read_FIFO
    #(parameter FIFO_DEPTH=32, FIFO_DEPTH_LOG2=5) 
(
    // Clock and reset.
    input wire clock,
    input wire reset_n,

    // Module controls.
    input wire z_active, // Whether to expect a Z read and to do a comparison.

    // Memory interface for reading Z pixels.
    input wire [63:0] read_readdata,
    input wire read_readdatavalid,

    // FIFO controls.
    input wire enqueue,
    input wire [28:0] color_address,
    input wire [63:0] color,
    input wire [28:0] z_address,
    input wire [63:0] z,
    input wire [1:0] pixel_active, // Bit 0 is the left-most pixel.
    output wire [FIFO_DEPTH_LOG2-1:0] size,

    // Write FIFO controls.
    output reg write_enqueue,
    output reg [28:0] write_color_address,
    output reg [63:0] write_color,
    output reg [28:0] write_z_address,
    output reg [63:0] write_z,
    output reg [1:0] write_pixel_active // Bit 0 is the left-most pixel.
);

    // Latched Z from memory.
    reg [63:0] memory_z;

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

    // Z computation.
    wire [31:0] fifo_z_0 = fifo_z[31:0];
    wire [31:0] fifo_z_1 = fifo_z[63:32];
    wire [31:0] memory_z_0 = memory_z[31:0];
    wire [31:0] memory_z_1 = memory_z[63:32];
    wire z_pass_0 = fifo_z_0 <= memory_z_0;
    wire z_pass_1 = fifo_z_1 <= memory_z_1;
    wire [1:0] new_pixel_active = {
        fifo_pixel_active[1] && z_pass_1,
        fifo_pixel_active[0] && z_pass_0
    };

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

    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            fifo_read <= 1'b0;
            write_enqueue <= 1'b0;
            memory_z <= 1'b0;
        end else begin
            if (!fifo_empty && (!z_active || read_readdatavalid)) begin
                fifo_read <= 1'b1;

                // Grab data from memory if it's available.
                if (z_active && read_readdatavalid) begin
                    memory_z <= read_readdata;
                end
            end else begin
                fifo_read <= 1'b0;
            end

            // See if we were reading in the previous clock.
            if (fifo_read) begin
                // Enqueue if Z if off, or if either new Z pixel is not behind
                // existing pixel.
                write_enqueue <= !z_active || |new_pixel_active;
                write_color_address <= fifo_color_address;
                write_color <= fifo_color;
                write_z_address <= fifo_z_address;
                write_z <= fifo_z;
                write_pixel_active <= new_pixel_active;
            end else begin
                write_enqueue <= 1'b0;
            end
        end
    end

endmodule
