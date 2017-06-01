// Takes a stream of graphics commands and performs the drawing
// operations on the frame buffer.

// Disables drawing the triangle pixels, which massively speeds up
// compilation.
/// `define DISABLE_DRAWING

module Rasterizer
    // In bytes.
    #(parameter FB_ADDRESS=0, FB_LENGTH=0, FB_WIDTH=0, CMD_ADDRESS=0)
(
    // Clock and reset.
    input wire clock,
    input wire reset_n,
    input wire test_mode,

    // Semaphores to data-producing module.
    input wire data_ready,
    output reg busy,

    // Memory interface for reading command buffer and Z pixels.
    output reg [28:0] read_address,
    output wire [7:0] read_burstcount,
    input wire read_waitrequest,
    input wire [63:0] read_readdata,
    input wire read_readdatavalid,
    output reg read_read,

    // Memory interface for writing color pixels.
    output wire [28:0] write_color_address,
    output wire [7:0] write_color_burstcount,
    input wire write_color_waitrequest,
    output wire [63:0] write_color_writedata,
    output wire [7:0] write_color_byteenable,
    output wire write_color_write,

    // Memory interface for writing Z pixels.
    output wire [28:0] write_z_address,
    output wire [7:0] write_z_burstcount,
    input wire write_z_waitrequest,
    output wire [63:0] write_z_writedata,
    output wire [7:0] write_z_byteenable,
    output wire write_z_write,

    // Front buffer handling.
    input wire fb_front_buffer,
    output reg rast_front_buffer,

    // These get displayed on the LCD.
    output wire [31:0] debug_value0,
    output wire [31:0] debug_value1,
    output wire [31:0] debug_value2
);

    // Debugging.
    assign debug_value0 = {
        3'b0,
        busy,
        3'b0,
        data_ready,
        2'b0,
        state,
        unhandled_count
    };
    assign debug_value1 = pc;
    assign debug_value2 = command_word[31:0];

    // Constants.
    assign read_burstcount = 8'h01;

    localparam WRITE_FIFO_DEPTH = 32;
    localparam WRITE_FIFO_DEPTH_LOG2 = 5;
    localparam BOTH_FIFO_MAX = WRITE_FIFO_DEPTH - 10;

    // Address of Z buffer. It's after the two color buffers.
    localparam Z_ADDRESS = FB_ADDRESS + 2*FB_LENGTH;

    // State machine.
    localparam STATE_INIT = 6'h00;
    localparam STATE_WAIT_FOR_DATA = 6'h01;
    localparam STATE_WAIT_FOR_NO_DATA = 6'h02;
    localparam STATE_READ_COMMAND = 6'h03;
    localparam STATE_WAIT_READ_COMMAND = 6'h04;
    localparam STATE_DECODE_COMMAND = 6'h05;
    localparam STATE_CMD_CLEAR = 6'h06;
    localparam STATE_CMD_CLEAR_LOOP = 6'h07;
    localparam STATE_CMD_ZCLEAR = 6'h08;
    localparam STATE_CMD_ZCLEAR_LOOP = 6'h09;
    localparam STATE_CMD_CZCLEAR = 6'h0A;
    localparam STATE_CMD_CZCLEAR_LOOP = 6'h0B;
    localparam STATE_CMD_PATTERN = 6'h0C;
    localparam STATE_CMD_PATTERN_WAIT_READ_0 = 6'h0D;
    localparam STATE_CMD_PATTERN_WAIT_READ_1 = 6'h0E;
    localparam STATE_CMD_PATTERN_WAIT_READ_2 = 6'h0F;
    localparam STATE_CMD_PATTERN_WAIT_READ_3 = 6'h10;
    localparam STATE_CMD_DRAW = 6'h11;
    localparam STATE_CMD_DRAW_TRIANGLE_READ_0 = 6'h12;
    localparam STATE_CMD_DRAW_TRIANGLE_WAIT_READ_0 = 6'h13;
    localparam STATE_CMD_DRAW_TRIANGLE_WAIT_READ_1 = 6'h14;
    localparam STATE_CMD_DRAW_TRIANGLE_WAIT_READ_2 = 6'h15;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE1 = 6'h16;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE2 = 6'h17;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE3 = 6'h18;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE4 = 6'h19;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE5 = 6'h1A;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE6 = 6'h1B;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE7 = 6'h1C;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE8 = 6'h1D;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE9 = 6'h1E;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE10 = 6'h1F;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE11 = 6'h20;
    localparam STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX_LOOP = 6'h21;
    localparam STATE_CMD_DRAW_TRIANGLE_FLUSH_FIFO = 6'h22;
    localparam STATE_CMD_SWAP = 6'h23;
    localparam STATE_CMD_SWAP_WAIT = 6'h24;
    localparam STATE_CMD_END = 6'h25;
    reg [5:0] state;

    // Protocol command number:
    localparam CMD_CLEAR = 8'd1;
    localparam CMD_ZCLEAR = 8'd2;
    localparam CMD_PATTERN = 8'd3;
    localparam CMD_DRAW = 8'd4;
    localparam CMD_BITMAP = 8'd5;
    localparam CMD_SWAP = 8'd6;
    localparam CMD_END = 8'd7;
    localparam CMD_CZCLEAR = 8'd8;

    reg [10:0] tmp_counter;

    // Program counter into the command buffer.
    reg [26:0] pc;
    reg [15:0] unhandled_count;

    // The most recent command word we've read.
    reg [63:0] command_word;
    wire [7:0] command = command_word[7:0];
    wire use_z_buffer = command_word[32];
    wire draw_with_pattern = command_word[33];
    wire [7:0] color_clear_red = command_word[63:56];
    wire [7:0] color_clear_green = command_word[55:48];
    wire [7:0] color_clear_blue = command_word[47:40];
    wire [31:0] z_clear_value = { 16'b0, command_word[31:16] };

    // Drawing state.
    reg [255:0] pattern;
    reg [15:0] triangle_count;
    // Current triangle's raw vertices:
    reg [63:0] vertex_0;
    reg [63:0] vertex_1;
    reg [63:0] vertex_2;
    // Pull out the vertex data (raw bits). Extend colors and Z to 32 bits so
    // that later multiplies will have sufficient precision.
    wire [9:0] vertex_0_x = vertex_0[11:2];
    wire [8:0] vertex_0_y = vertex_0[23:15];
    wire signed [31:0] vertex_0_z = {16'b0, vertex_0[39:24]};
    wire signed [31:0] vertex_0_red = {24'b0, vertex_0[63:56]};
    wire signed [31:0] vertex_0_green = {24'b0, vertex_0[55:48]};
    wire signed [31:0] vertex_0_blue = {24'b0, vertex_0[47:40]};
    wire [9:0] vertex_1_x = vertex_1[11:2];
    wire [8:0] vertex_1_y = vertex_1[23:15];
    wire signed [31:0] vertex_1_z = {16'b0, vertex_1[39:24]};
    wire signed [31:0] vertex_1_red = {24'b0, vertex_1[63:56]};
    wire signed [31:0] vertex_1_green = {24'b0, vertex_1[55:48]};
    wire signed [31:0] vertex_1_blue = {24'b0, vertex_1[47:40]};
    wire [9:0] vertex_2_x = vertex_2[11:2];
    wire [8:0] vertex_2_y = vertex_2[23:15];
    wire signed [31:0] vertex_2_z = {16'b0, vertex_2[39:24]};
    wire signed [31:0] vertex_2_red = {24'b0, vertex_2[63:56]};
    wire signed [31:0] vertex_2_green = {24'b0, vertex_2[55:48]};
    wire signed [31:0] vertex_2_blue = {24'b0, vertex_2[47:40]};
    // Signed versions of the vertices, so subtractions can get negative
    // results. Must prepend with a zero bit so that the MSB isn't
    // interpreted as a sign.
    wire signed [10:0] vertex_0_sx = $signed({1'b0, vertex_0_x});
    wire signed [9:0] vertex_0_sy = $signed({1'b0, vertex_0_y});
    wire signed [10:0] vertex_1_sx = $signed({1'b0, vertex_1_x});
    wire signed [9:0] vertex_1_sy = $signed({1'b0, vertex_1_y});
    wire signed [10:0] vertex_2_sx = $signed({1'b0, vertex_2_x});
    wire signed [9:0] vertex_2_sy = $signed({1'b0, vertex_2_y});
    reg [9:0] tri_x_0;
    reg [9:0] tri_x_1;
    reg [8:0] tri_y;
    reg [9:0] tri_min_x;
    reg [8:0] tri_min_y;
    reg [9:0] tri_max_x;
    reg [8:0] tri_max_y;
    wire signed [10:0] tri_min_sx = $signed({1'b0, tri_min_x});
    wire signed [9:0] tri_min_sy = $signed({1'b0, tri_min_y});
    wire signed [10:0] tri_max_sx = $signed({1'b0, tri_max_x});
    wire signed [9:0] tri_max_sy = $signed({1'b0, tri_max_y});
    reg [28:0] tri_color_address_row;
    reg [28:0] tri_color_address;
    reg [28:0] tri_z_address_row;
    reg [28:0] tri_z_address;
    // s31.0 format:
    // TODO could be smaller.
    reg signed [31:0] tri_area;
    // s0.31 format:
    reg signed [31:0] tri_area_recip;
    // Edge equations.
    reg signed [21:0] tri_w0_0;
    reg signed [21:0] tri_w1_0;
    reg signed [21:0] tri_w2_0;
    reg signed [21:0] tri_w0_1;
    reg signed [21:0] tri_w1_1;
    reg signed [21:0] tri_w2_1;
    reg signed [21:0] tri_w0_row;
    reg signed [21:0] tri_w1_row;
    reg signed [21:0] tri_w2_row;
    reg signed [10:0] tri_w0_incr;
    reg signed [10:0] tri_w1_incr;
    reg signed [10:0] tri_w2_incr;
    reg signed [10:0] tri_w0_row_incr;
    reg signed [10:0] tri_w1_row_incr;
    reg signed [10:0] tri_w2_row_incr;
    // Colors.
    reg signed [63:0] tri_red_0;
    reg signed [63:0] tri_red_1;
    reg signed [63:0] tri_red_row;
    reg signed [63:0] tri_red_incr;
    reg signed [63:0] tri_red_row_incr;
    reg signed [63:0] tri_green_0;
    reg signed [63:0] tri_green_1;
    reg signed [63:0] tri_green_row;
    reg signed [63:0] tri_green_incr;
    reg signed [63:0] tri_green_row_incr;
    reg signed [63:0] tri_blue_0;
    reg signed [63:0] tri_blue_1;
    reg signed [63:0] tri_blue_row;
    reg signed [63:0] tri_blue_incr;
    reg signed [63:0] tri_blue_row_incr;
    // Z.
    reg signed [63:0] tri_z_0;
    reg signed [63:0] tri_z_1;
    reg signed [63:0] tri_z_row;
    reg signed [63:0] tri_z_incr;
    reg signed [63:0] tri_z_row_incr;
    wire inside_triangle_0 = (((tri_w0_0 <= 0 && tri_w1_0 <= 0 && tri_w2_0 <= 0)
            || (tri_w0_0 >= 0 && tri_w1_0 >= 0 && tri_w2_0 >= 0))
        && (!draw_with_pattern || pattern[{tri_y[3:0], tri_x_0[3:0]}]))
        || test_mode;
    wire inside_triangle_1 = (((tri_w0_1 <= 0 && tri_w1_1 <= 0 && tri_w2_1 <= 0)
            || (tri_w0_1 >= 0 && tri_w1_1 >= 0 && tri_w2_1 >= 0))
        && (!draw_with_pattern || pattern[{tri_y[3:0], tri_x_1[3:0]}]))
        || test_mode;
    wire [28:0] upper_left_color_address =
        fb_address + ((tri_min_y*FB_WIDTH + tri_min_x) >> 1);
    wire [28:0] upper_left_z_address =
        Z_ADDRESS/8 + ((tri_min_y*FB_WIDTH + tri_min_x) >> 1);
    wire signed [21:0] initial_w0_row =
        (vertex_2_sx - vertex_1_sx)*(tri_min_sy - vertex_1_sy) -
        (vertex_2_sy - vertex_1_sy)*(tri_min_sx - vertex_1_sx);
    wire signed [21:0] initial_w1_row =
        (vertex_0_sx - vertex_2_sx)*(tri_min_sy - vertex_2_sy) -
        (vertex_0_sy - vertex_2_sy)*(tri_min_sx - vertex_2_sx);
    wire signed [21:0] initial_w2_row =
        (vertex_1_sx - vertex_0_sx)*(tri_min_sy - vertex_0_sy) -
        (vertex_1_sy - vertex_0_sy)*(tri_min_sx - vertex_0_sx);
    wire [7:0] tri_red_byte_0 = tri_red_0[38:31];
    wire [7:0] tri_green_byte_0 = tri_green_0[38:31];
    wire [7:0] tri_blue_byte_0 = tri_blue_0[38:31];
    wire [31:0] tri_z_value_0 = tri_z_0[62:31];
    wire [7:0] tri_red_byte_1 = tri_red_1[38:31];
    wire [7:0] tri_green_byte_1 = tri_green_1[38:31];
    wire [7:0] tri_blue_byte_1 = tri_blue_1[38:31];
    wire [31:0] tri_z_value_1 = tri_z_1[62:31];

    wire write_fifo_enqueue;
    wire [28:0] write_fifo_color_address;
    wire [63:0] write_fifo_color;
    wire [28:0] write_fifo_z_address;
    wire [63:0] write_fifo_z;
    wire [1:0] write_fifo_pixel_active;
    wire [WRITE_FIFO_DEPTH_LOG2-1:0] write_fifo_size;
    wire [7:0] write_fifo_error;

    // Divider for the area reciprocal.
    // https://www.altera.com/en_US/pdfs/literature/ug/ug_lpm_alt_mfug.pdf
    wire signed [31:0] tri_area_recip_result;
    reg area_reciprocal_enabled;
    lpm_divide
        #(.LPM_WIDTHN(32),
          .LPM_WIDTHD(32),
          .LPM_NREPRESENTATION("UNSIGNED"),
          .LPM_DREPRESENTATION("SIGNED"),
          .LPM_PIPELINE(6)) area_divider(
            .clock(clock),
            .clken(area_reciprocal_enabled),
            .numer(32'h7FFF_FFFF),
            .denom(tri_area),
            .quotient(tri_area_recip_result)
        );

    // Read FIFO for waiting for Z read results.
    reg read_fifo_z_active;
    reg read_fifo_enqueue;
    reg [28:0] read_fifo_color_address;
    reg [63:0] read_fifo_color;
    reg [28:0] read_fifo_z_address;
    reg [63:0] read_fifo_z;
    reg [1:0] read_fifo_pixel_active;
    wire [WRITE_FIFO_DEPTH_LOG2-1:0] read_fifo_size;
    wire [WRITE_FIFO_DEPTH_LOG2-1:0] both_fifo_size = read_fifo_size + write_fifo_size;
    Read_FIFO
        #(.FIFO_DEPTH(WRITE_FIFO_DEPTH),
          .FIFO_DEPTH_LOG2(WRITE_FIFO_DEPTH_LOG2)) read_fifo(

            .clock(clock),
            .reset_n(reset_n),
            .z_active(read_fifo_z_active),

            // Memory interface for reading z pixels.
            .read_readdata(read_readdata),
            .read_readdatavalid(read_readdatavalid),

            // FIFO controls.
            .enqueue(read_fifo_enqueue),
            .color_address(read_fifo_color_address),
            .color(read_fifo_color),
            .z_address(read_fifo_z_address),
            .z(read_fifo_z),
            .pixel_active(read_fifo_pixel_active),
            .size(read_fifo_size),

            // Write FIFO controls.
            .write_enqueue(write_fifo_enqueue),
            .write_color_address(write_fifo_color_address),
            .write_color(write_fifo_color),
            .write_z_address(write_fifo_z_address),
            .write_z(write_fifo_z),
            .write_pixel_active(write_fifo_pixel_active)
        );

    // Write FIFO for writing pixels to memory.
    Write_FIFO
        #(.FIFO_DEPTH(WRITE_FIFO_DEPTH),
          .FIFO_DEPTH_LOG2(WRITE_FIFO_DEPTH_LOG2)) write_fifo(

            .clock(clock),
            .reset_n(reset_n),
            .error(write_fifo_error),

            // Memory interface for writing color pixels.
            .write_color_address(write_color_address),
            .write_color_burstcount(write_color_burstcount),
            .write_color_waitrequest(write_color_waitrequest),
            .write_color_writedata(write_color_writedata),
            .write_color_byteenable(write_color_byteenable),
            .write_color_write(write_color_write),

            // Memory interface for writing Z pixels.
            .write_z_address(write_z_address),
            .write_z_burstcount(write_z_burstcount),
            .write_z_waitrequest(write_z_waitrequest),
            .write_z_writedata(write_z_writedata),
            .write_z_byteenable(write_z_byteenable),
            .write_z_write(write_z_write),

            // FIFO controls.
            .enqueue(write_fifo_enqueue),
            .color_address(write_fifo_color_address),
            .color(write_fifo_color),
            .z_address(write_fifo_z_address),
            .z(write_fifo_z),
            .pixel_active(write_fifo_pixel_active),
            .size(write_fifo_size)
        );

    // We draw to the back buffer.
    wire [28:0] fb_address = rast_front_buffer ? FB_ADDRESS/8 : (FB_ADDRESS + FB_LENGTH)/8;

    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            // State.
            state <= STATE_INIT;
            busy <= 1'b0;
            pc <= 1'b0;
            unhandled_count <= 1'b0;
            triangle_count <= 1'b0;
            tri_x_0 <= 1'b0;
            tri_x_1 <= 1'b0;
            tri_y <= 1'b0;
            tri_min_x <= 1'b0;
            tri_min_y <= 1'b0;
            tri_max_x <= 1'b0;
            tri_max_y <= 1'b0;
            rast_front_buffer <= 1'b0;
            pattern <= 1'b0;
            area_reciprocal_enabled <= 1'b0;

            // Memory.
            read_address <= 1'b0;
            read_read <= 1'b0;
        end else begin
            /// if (write_fifo_error != 0) begin
            ///     unhandled_count <= write_fifo_error;
            /// end

            case (state)
                STATE_INIT: begin
                    busy <= 1'b0;
                    state <= STATE_WAIT_FOR_DATA;
                end

                STATE_WAIT_FOR_DATA: begin
                    if (data_ready) begin
                        state <= STATE_WAIT_FOR_NO_DATA;
                        busy <= 1'b1;
                    end
                end

                STATE_WAIT_FOR_NO_DATA: begin
                    if (!data_ready) begin
                        state <= STATE_READ_COMMAND;
                        pc <= CMD_ADDRESS/8;
                    end
                end

                STATE_READ_COMMAND: begin
                    read_address <= pc;
                    read_read <= 1'b1;
                    pc <= pc + 1'b1;
                    state <= STATE_WAIT_READ_COMMAND;
                end

                STATE_WAIT_READ_COMMAND: begin
                    // When no longer told to wait, deassert the request lines.
                    if (!read_waitrequest) begin
                        read_read <= 1'b0;
                    end

                    // If we have data, grab it.
                    if (read_readdatavalid) begin
                        command_word <= read_readdata;
                        state <= STATE_DECODE_COMMAND;
                    end
                end

                STATE_DECODE_COMMAND: begin
                    case (command)
                        CMD_CLEAR: begin
                            state <= STATE_CMD_CLEAR;
                        end

                        CMD_ZCLEAR: begin
                            state <= STATE_CMD_ZCLEAR;
                        end

                        CMD_PATTERN: begin
                            state <= STATE_CMD_PATTERN;
                        end

                        CMD_DRAW: begin
                            state <= STATE_CMD_DRAW;
                        end

                        CMD_SWAP: begin
                            state <= STATE_CMD_SWAP;
                        end

                        CMD_END: begin
                            state <= STATE_CMD_END;
                        end

                        CMD_CZCLEAR: begin
                            state <= STATE_CMD_CZCLEAR;
                        end

                        default: begin
                            // Unhandled command, abort.
                            unhandled_count <= unhandled_count + 1'b1;
			    // grantham : on unexpected command, don't
			    // transition to any more states -
			    // PC will be in debug1 and command will
			    // be in debug2
                            // state <= STATE_INIT;
                        end
                    endcase
                end

                STATE_CMD_CLEAR: begin
                    if (both_fifo_size < BOTH_FIFO_MAX) begin
                        read_fifo_z_active <= 1'b0;
                        read_fifo_color_address <= fb_address;
                        read_fifo_color <= {
                            8'b0,
                            color_clear_blue,
                            color_clear_green,
                            color_clear_red,
                            8'b0,
                            color_clear_blue,
                            color_clear_green,
                            color_clear_red
                        };
                        read_fifo_z_address <= 1'b0;
                        read_fifo_z <= 1'b0;
                        read_fifo_pixel_active <= 2'b11;
                        read_fifo_enqueue <= 1'b1;
                        state <= STATE_CMD_CLEAR_LOOP;
                    end
                end

                STATE_CMD_CLEAR_LOOP: begin
                    if (read_fifo_color_address == fb_address + FB_LENGTH/8 - 1) begin
                        read_fifo_enqueue <= 1'b0;
                        state <= STATE_READ_COMMAND;
                    end else begin
                        if (both_fifo_size < BOTH_FIFO_MAX) begin
                            read_fifo_color_address <= read_fifo_color_address + 1'b1;
                            read_fifo_enqueue <= 1'b1;
                        end else begin
                            read_fifo_enqueue <= 1'b0;
                        end
                    end
                end

                STATE_CMD_ZCLEAR: begin
                    if (both_fifo_size < BOTH_FIFO_MAX) begin
                        read_fifo_z_active <= 1'b0;
                        read_fifo_color_address <= 1'b0;
                        read_fifo_color <= 1'b0;
                        read_fifo_z_address <= Z_ADDRESS/8;
                        read_fifo_z <= { z_clear_value, z_clear_value };
                        read_fifo_pixel_active <= 2'b11;
                        read_fifo_enqueue <= 1'b1;
                        state <= STATE_CMD_ZCLEAR_LOOP;
                    end
                end

                STATE_CMD_ZCLEAR_LOOP: begin
                    if (read_fifo_z_address == Z_ADDRESS/8 + FB_LENGTH/8 - 1) begin
                        read_fifo_enqueue <= 1'b0;
                        state <= STATE_READ_COMMAND;
                    end else begin
                        if (both_fifo_size < BOTH_FIFO_MAX) begin
                            read_fifo_z_address <= read_fifo_z_address + 1'b1;
                            read_fifo_enqueue <= 1'b1;
                        end else begin
                            read_fifo_enqueue <= 1'b0;
                        end
                    end
                end

                STATE_CMD_CZCLEAR: begin
                    if (both_fifo_size < BOTH_FIFO_MAX) begin
                        read_fifo_z_active <= 1'b0;
                        read_fifo_color_address <= fb_address;
                        read_fifo_color <= {
                            8'b0,
                            color_clear_blue,
                            color_clear_green,
                            color_clear_red,
                            8'b0,
                            color_clear_blue,
                            color_clear_green,
                            color_clear_red
                        };
                        read_fifo_z_address <= Z_ADDRESS/8;
                        read_fifo_z <= { z_clear_value, z_clear_value };
                        read_fifo_pixel_active <= 2'b11;
                        read_fifo_enqueue <= 1'b1;
                        state <= STATE_CMD_CZCLEAR_LOOP;
                    end
                end

                STATE_CMD_CZCLEAR_LOOP: begin
                    if (read_fifo_z_address == Z_ADDRESS/8 + FB_LENGTH/8 - 1) begin
                        read_fifo_enqueue <= 1'b0;
                        state <= STATE_READ_COMMAND;
                    end else begin
                        if (both_fifo_size < BOTH_FIFO_MAX) begin
                            read_fifo_color_address <= read_fifo_color_address + 1'b1;
                            read_fifo_z_address <= read_fifo_z_address + 1'b1;
                            read_fifo_enqueue <= 1'b1;
                        end else begin
                            read_fifo_enqueue <= 1'b0;
                        end
                    end
                end

                STATE_CMD_PATTERN: begin
                    // Read first pattern word.
                    read_address <= pc;
                    read_read <= 1'b1;
                    pc <= pc + 1'b1;
                    state <= STATE_CMD_PATTERN_WAIT_READ_0;
                end

                STATE_CMD_PATTERN_WAIT_READ_0: begin
                    // When no longer told to wait, deassert the request lines.
                    if (!read_waitrequest) begin
                        read_read <= 1'b0;
                    end

                    // If we have data, grab it.
                    if (read_readdatavalid) begin
                        pattern[63:0] <= read_readdata;
                        read_address <= pc;
                        read_read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_PATTERN_WAIT_READ_1;
                    end
                end

                STATE_CMD_PATTERN_WAIT_READ_1: begin
                    // When no longer told to wait, deassert the request lines.
                    if (!read_waitrequest) begin
                        read_read <= 1'b0;
                    end

                    // If we have data, grab it.
                    if (read_readdatavalid) begin
                        pattern[127:64] <= read_readdata;
                        read_address <= pc;
                        read_read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_PATTERN_WAIT_READ_2;
                    end
                end

                STATE_CMD_PATTERN_WAIT_READ_2: begin
                    // When no longer told to wait, deassert the request lines.
                    if (!read_waitrequest) begin
                        read_read <= 1'b0;
                    end

                    // If we have data, grab it.
                    if (read_readdatavalid) begin
                        pattern[191:128] <= read_readdata;
                        read_address <= pc;
                        read_read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_PATTERN_WAIT_READ_3;
                    end
                end

                STATE_CMD_PATTERN_WAIT_READ_3: begin
                    // When no longer told to wait, deassert the request lines.
                    if (!read_waitrequest) begin
                        read_read <= 1'b0;
                    end

                    // If we have data, grab it.
                    if (read_readdatavalid) begin
                        pattern[255:192] <= read_readdata;
                        state <= STATE_READ_COMMAND;
                    end
                end

                STATE_CMD_DRAW: begin
                    // We only handle triangles, so no point in checking
                    // the type.
                    triangle_count <= command_word[31:16];
                    read_fifo_z_active <= use_z_buffer;
                    state <= STATE_CMD_DRAW_TRIANGLE_READ_0;
                end

                STATE_CMD_DRAW_TRIANGLE_READ_0: begin
                    if (triangle_count == 1'b0) begin
                        // Done with all the triangles we had to draw.
                        state <= STATE_READ_COMMAND;
                    end else begin
                        triangle_count <= triangle_count - 1'b1;

                        // Read first vertex.
                        read_address <= pc;
                        read_read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_DRAW_TRIANGLE_WAIT_READ_0;
                    end
                end

                STATE_CMD_DRAW_TRIANGLE_WAIT_READ_0: begin
                    if (!read_waitrequest && !read_readdatavalid) begin
                        read_read <= 1'b0;
                    end

                    if (read_readdatavalid) begin
                        vertex_0 <= read_readdata;
                        read_address <= pc;
                        read_read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_DRAW_TRIANGLE_WAIT_READ_1;
                    end
                end

                STATE_CMD_DRAW_TRIANGLE_WAIT_READ_1: begin
                    if (!read_waitrequest && !read_readdatavalid) begin
                        read_read <= 1'b0;
                    end

                    if (read_readdatavalid) begin
                        vertex_1 <= read_readdata;
                        read_address <= pc;
                        read_read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_DRAW_TRIANGLE_WAIT_READ_2;
                    end
                end

                STATE_CMD_DRAW_TRIANGLE_WAIT_READ_2: begin
                    if (!read_waitrequest) begin
                        read_read <= 1'b0;
                    end

                    if (read_readdatavalid) begin
                        vertex_2 <= read_readdata;
                        state <= STATE_CMD_DRAW_TRIANGLE_PREPARE1;
                    end
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE1: begin
                    // Compute bounding box, rounding to even pixels.
                    tri_min_x <= ((vertex_0_x < vertex_1_x)
                                    ? (vertex_0_x < vertex_2_x)
                                        ? vertex_0_x 
                                        : vertex_2_x
                                    : (vertex_1_x < vertex_2_x)
                                        ? vertex_1_x
                                        : vertex_2_x) & ~10'b1;
                    tri_min_y <= (vertex_0_y < vertex_1_y)
                                    ? (vertex_0_y < vertex_2_y)
                                        ? vertex_0_y 
                                        : vertex_2_y
                                    : (vertex_1_y < vertex_2_y)
                                        ? vertex_1_y
                                        : vertex_2_y;
                    tri_max_x <= ((vertex_0_x > vertex_1_x)
                                    ? (vertex_0_x > vertex_2_x)
                                        ? vertex_0_x 
                                        : vertex_2_x
                                    : (vertex_1_x > vertex_2_x)
                                        ? vertex_1_x
                                        : vertex_2_x) & ~10'b1;
                    tri_max_y <= (vertex_0_y > vertex_1_y)
                                    ? (vertex_0_y > vertex_2_y)
                                        ? vertex_0_y 
                                        : vertex_2_y
                                    : (vertex_1_y > vertex_2_y)
                                        ? vertex_1_y
                                        : vertex_2_y;
                    tri_w0_incr <= vertex_1_sy - vertex_2_sy;
                    tri_w0_row_incr <= vertex_2_sx - vertex_1_sx;
                    tri_w1_incr <= vertex_2_sy - vertex_0_sy;
                    tri_w1_row_incr <= vertex_0_sx - vertex_2_sx;
                    tri_w2_incr <= vertex_0_sy - vertex_1_sy;
                    tri_w2_row_incr <= vertex_1_sx - vertex_0_sx;
                    tri_area <= 
                        (vertex_1_sx - vertex_0_sx)*
                        (vertex_2_sy - vertex_0_sy) -
                        (vertex_1_sy - vertex_0_sy)*
                        (vertex_2_sx - vertex_0_sx);
                    area_reciprocal_enabled <= 1'b1;
                    state <= STATE_CMD_DRAW_TRIANGLE_PREPARE2;
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE2: begin
                    // Reciprocal latency 0.
                    if (tri_area == 0) begin
                        // Empty triangle.
                        area_reciprocal_enabled <= 1'b0;
                        state <= STATE_CMD_DRAW_TRIANGLE_READ_0;
                    end else begin
                        tri_w0_row <= initial_w0_row;
                        tri_w1_row <= initial_w1_row;
                        tri_w2_row <= initial_w2_row;
                        tri_w0_0 <= initial_w0_row;
                        tri_w1_0 <= initial_w1_row;
                        tri_w2_0 <= initial_w2_row;
                        tri_w0_1 <= initial_w0_row + tri_w0_incr;
                        tri_w1_1 <= initial_w1_row + tri_w1_incr;
                        tri_w2_1 <= initial_w2_row + tri_w2_incr;
                        tri_x_0 <= tri_min_x;
                        tri_x_1 <= tri_min_x + 1'b1;
                        tri_y <= tri_min_y;
                        tri_color_address_row <= upper_left_color_address;
                        tri_color_address <= upper_left_color_address;
                        tri_z_address_row <= upper_left_z_address;
                        tri_z_address <= upper_left_z_address;
                        state <= STATE_CMD_DRAW_TRIANGLE_PREPARE3;
                    end
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE3: begin
                    // Reciprocal latency 1.
                    state <= STATE_CMD_DRAW_TRIANGLE_PREPARE4;
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE4: begin
                    // Reciprocal latency 2.
                    state <= STATE_CMD_DRAW_TRIANGLE_PREPARE5;
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE5: begin
                    // Reciprocal latency 3.
                    state <= STATE_CMD_DRAW_TRIANGLE_PREPARE6;
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE6: begin
                    // Reciprocal latency 4.
                    state <= STATE_CMD_DRAW_TRIANGLE_PREPARE7;
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE7: begin
                    // Reciprocal latency 5.
                    state <= STATE_CMD_DRAW_TRIANGLE_PREPARE8;
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE8: begin
                    // Reciprocal latency 6.
                    area_reciprocal_enabled <= 1'b0;
                    tri_area_recip <= tri_area_recip_result;
                    state <= STATE_CMD_DRAW_TRIANGLE_PREPARE9;
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE9: begin
                    tri_red_row <=
                        tri_w0_row*vertex_0_red +
                        tri_w1_row*vertex_1_red +
                        tri_w2_row*vertex_2_red;
                    tri_red_incr <=
                        tri_w0_incr*vertex_0_red +
                        tri_w1_incr*vertex_1_red +
                        tri_w2_incr*vertex_2_red;
                    tri_red_row_incr <=
                        tri_w0_row_incr*vertex_0_red +
                        tri_w1_row_incr*vertex_1_red +
                        tri_w2_row_incr*vertex_2_red;
                    tri_green_row <=
                        tri_w0_row*vertex_0_green +
                        tri_w1_row*vertex_1_green +
                        tri_w2_row*vertex_2_green;
                    tri_green_incr <=
                        tri_w0_incr*vertex_0_green +
                        tri_w1_incr*vertex_1_green +
                        tri_w2_incr*vertex_2_green;
                    tri_green_row_incr <=
                        tri_w0_row_incr*vertex_0_green +
                        tri_w1_row_incr*vertex_1_green +
                        tri_w2_row_incr*vertex_2_green;
                    tri_blue_row <=
                        tri_w0_row*vertex_0_blue +
                        tri_w1_row*vertex_1_blue +
                        tri_w2_row*vertex_2_blue;
                    tri_blue_incr <=
                        tri_w0_incr*vertex_0_blue +
                        tri_w1_incr*vertex_1_blue +
                        tri_w2_incr*vertex_2_blue;
                    tri_blue_row_incr <=
                        tri_w0_row_incr*vertex_0_blue +
                        tri_w1_row_incr*vertex_1_blue +
                        tri_w2_row_incr*vertex_2_blue;
                    tri_z_row <=
                        tri_w0_row*vertex_0_z +
                        tri_w1_row*vertex_1_z +
                        tri_w2_row*vertex_2_z;
                    tri_z_incr <=
                        tri_w0_incr*vertex_0_z +
                        tri_w1_incr*vertex_1_z +
                        tri_w2_incr*vertex_2_z;
                    tri_z_row_incr <=
                        tri_w0_row_incr*vertex_0_z +
                        tri_w1_row_incr*vertex_1_z +
                        tri_w2_row_incr*vertex_2_z;
                    state <= STATE_CMD_DRAW_TRIANGLE_PREPARE10;
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE10: begin
                    tri_red_row <= tri_red_row*tri_area_recip;
                    tri_red_incr <= tri_red_incr*tri_area_recip;
                    tri_red_row_incr <= tri_red_row_incr*tri_area_recip;
                    tri_green_row <= tri_green_row*tri_area_recip;
                    tri_green_incr <= tri_green_incr*tri_area_recip;
                    tri_green_row_incr <= tri_green_row_incr*tri_area_recip;
                    tri_blue_row <= tri_blue_row*tri_area_recip;
                    tri_blue_incr <= tri_blue_incr*tri_area_recip;
                    tri_blue_row_incr <= tri_blue_row_incr*tri_area_recip;
                    tri_z_row <= tri_z_row*tri_area_recip;
                    tri_z_incr <= tri_z_incr*tri_area_recip;
                    tri_z_row_incr <= tri_z_row_incr*tri_area_recip;
                    state <= STATE_CMD_DRAW_TRIANGLE_PREPARE11;
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE11: begin
                    tri_red_0 <= tri_red_row;
                    tri_red_1 <= tri_red_row + tri_red_incr;
                    tri_green_0 <= tri_green_row;
                    tri_green_1 <= tri_green_row + tri_green_incr;
                    tri_blue_0 <= tri_blue_row;
                    tri_blue_1 <= tri_blue_row + tri_blue_incr;
                    tri_z_0 <= tri_z_row;
                    tri_z_1 <= tri_z_row + tri_z_incr;
                    state <= STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX_LOOP;
                end

                STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX_LOOP: begin
                    if (use_z_buffer && read_read && read_waitrequest) begin
                        // We asked to read from the Z pixel, but the memory
                        // controller is asking us to wait. Do nothing.

                        read_fifo_enqueue <= 1'b0;
                    end else begin
                        // Turn off Z pixel reading.
                        read_read <= 1'b0;

                        // Don't enqueue more than either FIFO can handle.
                        if (both_fifo_size < BOTH_FIFO_MAX) begin
                            // Decide whether to draw this pixel.
                            read_fifo_enqueue <= inside_triangle_0 || inside_triangle_1;
                            read_fifo_pixel_active <= { inside_triangle_1, inside_triangle_0 };
                            read_fifo_color_address <= tri_color_address;
`ifdef DISABLE_DRAWING
                            read_fifo_color <= 64'h00FFFF00_00FFFF00;
`else
                            read_fifo_color <= {
                                // Pixel 1.
                                8'h00,
                                tri_blue_byte_1,
                                tri_green_byte_1,
                                tri_red_byte_1,
                                // Pixel 0.
                                8'h00,
                                tri_blue_byte_0,
                                tri_green_byte_0,
                                tri_red_byte_0
                            };
`endif
                            read_fifo_z_address <= use_z_buffer ? tri_z_address : 1'b0;
`ifdef DISABLE_DRAWING
                            read_fifo_z <= 64'b0;
`else
                            read_fifo_z <= { tri_z_value_1, tri_z_value_0 };
`endif

                            // Initiate a read for the Z pixel.
                            read_read <= use_z_buffer && (inside_triangle_0 || inside_triangle_1);
                            read_address <= tri_z_address;

                            // Advance to the next pixel.
                            if (tri_x_0 == tri_max_x) begin
                                if (tri_y == tri_max_y) begin
                                    // Next triangle. We might still turn on
                                    // write this clock, but we'll turn
                                    // it off in the next state.
                                    tmp_counter <= 1'b0;
                                    state <= STATE_CMD_DRAW_TRIANGLE_FLUSH_FIFO;
                                end else begin
                                    // Next row.
                                    tri_x_0 <= tri_min_x;
                                    tri_x_1 <= tri_min_x + 1'b1;
                                    tri_y <= tri_y + 1'b1;
                                    tri_color_address <= tri_color_address_row;
                                    tri_color_address_row <= tri_color_address_row + FB_WIDTH/2;
                                    tri_z_address <= tri_z_address_row;
                                    tri_z_address_row <= tri_z_address_row + FB_WIDTH/2;
                                    tri_w0_0 <= tri_w0_row;
                                    tri_w1_0 <= tri_w1_row;
                                    tri_w2_0 <= tri_w2_row;
                                    tri_w0_1 <= tri_w0_row + tri_w0_incr;
                                    tri_w1_1 <= tri_w1_row + tri_w1_incr;
                                    tri_w2_1 <= tri_w2_row + tri_w2_incr;
                                    tri_w0_row <= tri_w0_row + tri_w0_row_incr;
                                    tri_w1_row <= tri_w1_row + tri_w1_row_incr;
                                    tri_w2_row <= tri_w2_row + tri_w2_row_incr;
                                    tri_red_0 <= tri_red_row;
                                    tri_red_1 <= tri_red_row + tri_red_incr;
                                    tri_green_0 <= tri_green_row;
                                    tri_green_1 <= tri_green_row + tri_green_incr;
                                    tri_blue_0 <= tri_blue_row;
                                    tri_blue_1 <= tri_blue_row + tri_blue_incr;
                                    tri_z_0 <= tri_z_row;
                                    tri_z_1 <= tri_z_row + tri_z_incr;
                                    tri_red_row <= tri_red_row + tri_red_row_incr;
                                    tri_green_row <= tri_green_row + tri_green_row_incr;
                                    tri_blue_row <= tri_blue_row + tri_blue_row_incr;
                                    tri_z_row <= tri_z_row + tri_z_row_incr;
                                end
                            end else begin
                                // Next pixel on this row.
                                tri_color_address <= tri_color_address + 1'b1;
                                tri_z_address <= tri_z_address + 1'b1;
                                tri_x_0 <= tri_x_0 + 2'd2;
                                tri_x_1 <= tri_x_1 + 2'd2;
                                tri_w0_0 <= tri_w0_0 + (tri_w0_incr << 1);
                                tri_w1_0 <= tri_w1_0 + (tri_w1_incr << 1);
                                tri_w2_0 <= tri_w2_0 + (tri_w2_incr << 1);
                                tri_w0_1 <= tri_w0_1 + (tri_w0_incr << 1);
                                tri_w1_1 <= tri_w1_1 + (tri_w1_incr << 1);
                                tri_w2_1 <= tri_w2_1 + (tri_w2_incr << 1);
                                tri_red_0 <= tri_red_0 + (tri_red_incr << 1);
                                tri_red_1 <= tri_red_1 + (tri_red_incr << 1);
                                tri_green_0 <= tri_green_0 + (tri_green_incr << 1);
                                tri_green_1 <= tri_green_1 + (tri_green_incr << 1);
                                tri_blue_0 <= tri_blue_0 + (tri_blue_incr << 1);
                                tri_blue_1 <= tri_blue_1 + (tri_blue_incr << 1);
                                tri_z_0 <= tri_z_0 + (tri_z_incr << 1);
                                tri_z_1 <= tri_z_1 + (tri_z_incr << 1);
                            end
                        end else begin
                            // FIFO full, blocked.
                            read_fifo_enqueue <= 1'b0;
                        end
                    end
                end

                STATE_CMD_DRAW_TRIANGLE_FLUSH_FIFO: begin
                    // Turn off write, we may have left it on for
                    // the lower-right pixel of the triangle's
                    // bounding box.
                    read_fifo_enqueue <= 1'b0;

                    // We jumped here after the last pixel, so we must make
                    // sure that we respect the wait request first.
                    if (read_read && read_waitrequest) begin
                        // We asked to read from the Z pixel, but the memory
                        // controller is asking us to wait. Do nothing.
                    end else begin
                        // Turn off Z pixel reading.
                        read_read <= 1'b0;

                        // We must wait for the read and write FIFOs to flush
                        // completely, for two reasons:
                        //
                        // We must flush the read FIFO because we have
                        // outstanding reads in the "read" memory port, and
                        // those would interfere with the subsequent vertex
                        // or command read.
                        //
                        // We must flush the write FIFO because there are
                        // Z pixels waiting to be written, and if we start
                        // a new triangle, we might read a stale Z
                        // value from memory.
                        //
                        // TODO: There may be a bug here where the sum of the
                        // sizes is 0 when the data is between the two FIFOs.
                        if ((both_fifo_size == 0 && !read_fifo_enqueue) || tmp_counter == 11'd1000) begin
                            /// unhandled_count <= tmp_counter;
                            state <= STATE_CMD_DRAW_TRIANGLE_READ_0;
                        end
                        tmp_counter <= tmp_counter + 1'b1;
                    end
                end

                STATE_CMD_SWAP: begin
                    // Request a swap of buffers.
                    rast_front_buffer <= !rast_front_buffer;
                    state <= STATE_CMD_SWAP_WAIT;
                end

                STATE_CMD_SWAP_WAIT: begin
                    // Block until the frame buffer has swapped.
                    if (rast_front_buffer == fb_front_buffer) begin
                        state <= STATE_READ_COMMAND;
                    end
                end

                STATE_CMD_END: begin
                    state <= STATE_INIT;
                end

                default: begin
                    // Bug, restart.
                    state <= STATE_INIT;
                end
            endcase
        end
    end

endmodule
