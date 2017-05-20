// Takes a stream of graphics commands and performs the drawing
// operations on the frame buffer.
module Rasterizer
    // In bytes.
    #(parameter FB_ADDRESS=0, FB_LENGTH=0, FB_WIDTH=0, CMD_ADDRESS=0)
(
    // Clock and reset.
    input clock,
    input wire reset_n,
    input test_mode,

    // Semaphores to data-producing module.
    input data_ready,
    output reg busy,

    // Memory interface.
    output reg [28:0] address,
    output wire [7:0] burstcount,
    input wire waitrequest,
    input wire [63:0] readdata,
    input wire readdatavalid,
    output reg read,
    output reg [63:0] writedata,
    output reg [7:0] byteenable,
    output reg write,

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
    assign debug_value1 = tri_area;
    assign debug_value2 = tri_area_recip;

    // Constants.
    assign burstcount = 8'h01;

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
    localparam STATE_CMD_PATTERN = 6'h0A;
    localparam STATE_CMD_PATTERN_WAIT_READ_0 = 6'h0B;
    localparam STATE_CMD_PATTERN_WAIT_READ_1 = 6'h0C;
    localparam STATE_CMD_PATTERN_WAIT_READ_2 = 6'h0D;
    localparam STATE_CMD_PATTERN_WAIT_READ_3 = 6'h0E;
    localparam STATE_CMD_DRAW = 6'h0F;
    localparam STATE_CMD_DRAW_TRIANGLE_READ_0 = 6'h10;
    localparam STATE_CMD_DRAW_TRIANGLE_WAIT_READ_0 = 6'h11;
    localparam STATE_CMD_DRAW_TRIANGLE_WAIT_READ_1 = 6'h12;
    localparam STATE_CMD_DRAW_TRIANGLE_WAIT_READ_2 = 6'h13;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE1 = 6'h14;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE2 = 6'h15;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE3 = 6'h16;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE4 = 6'h17;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE5 = 6'h18;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE6 = 6'h19;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE7 = 6'h1A;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE8 = 6'h1B;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE9 = 6'h1C;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE10 = 6'h1D;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE11 = 6'h1E;
    localparam STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX_LOOP = 6'h1F;
    localparam STATE_CMD_SWAP = 6'h20;
    localparam STATE_CMD_SWAP_WAIT = 6'h21;
    localparam STATE_CMD_END = 6'h22;
    reg [5:0] state;

    // Protocol command number:
    localparam CMD_CLEAR = 8'd1;
    localparam CMD_ZCLEAR = 8'd2;
    localparam CMD_PATTERN = 8'd3;
    localparam CMD_DRAW = 8'd4;
    localparam CMD_BITMAP = 8'd5;
    localparam CMD_SWAP = 8'd6;
    localparam CMD_END = 8'd7;

    // Program counter into the protocol buffer.
    reg [26:0] pc;
    reg [15:0] unhandled_count;

    // The most recent command word we've read.
    reg [63:0] command_word;
    wire [7:0] command = command_word[7:0];
    wire use_z_buffer = command_word[32];
    wire draw_with_pattern = command_word[33];
    wire [31:0] z_clear_value = { 16'b0, command_word[31:16] };

    // Drawing state.
    reg [255:0] pattern;
    reg [15:0] triangle_count;
    // Current triangle's raw vertices:
    reg [63:0] vertex_0;
    reg [63:0] vertex_1;
    reg [63:0] vertex_2;
    // Pull out the vertex data (raw bits):
    wire [9:0] vertex_0_x = vertex_0[11:2];
    wire [8:0] vertex_0_y = vertex_0[23:15];
    // Extend colors to 32 bits so that later multiplies will
    // have sufficient precision.
    wire signed [31:0] vertex_0_red = {24'b0, vertex_0[63:56]};
    wire signed [31:0] vertex_0_green = {24'b0, vertex_0[55:48]};
    wire signed [31:0] vertex_0_blue = {24'b0, vertex_0[47:40]};
    wire [9:0] vertex_1_x = vertex_1[11:2];
    wire [8:0] vertex_1_y = vertex_1[23:15];
    wire signed [31:0] vertex_1_red = {24'b0, vertex_1[63:56]};
    wire signed [31:0] vertex_1_green = {24'b0, vertex_1[55:48]};
    wire signed [31:0] vertex_1_blue = {24'b0, vertex_1[47:40]};
    wire [9:0] vertex_2_x = vertex_2[11:2];
    wire [8:0] vertex_2_y = vertex_2[23:15];
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
    reg [28:0] tri_address_row;
    reg [28:0] tri_address;
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
    wire inside_triangle_0 = (((tri_w0_0 <= 0 && tri_w1_0 <= 0 && tri_w2_0 <= 0)
            || (tri_w0_0 >= 0 && tri_w1_0 >= 0 && tri_w2_0 >= 0))
        && (!draw_with_pattern || pattern[{tri_y[3:0], tri_x_0[3:0]}]))
        || test_mode;
    wire inside_triangle_1 = (((tri_w0_1 <= 0 && tri_w1_1 <= 0 && tri_w2_1 <= 0)
            || (tri_w0_1 >= 0 && tri_w1_1 >= 0 && tri_w2_1 >= 0))
        && (!draw_with_pattern || pattern[{tri_y[3:0], tri_x_1[3:0]}]))
        || test_mode;
    wire [28:0] upper_left_address =
        fb_address + ((tri_min_y*FB_WIDTH + tri_min_x) >> 1);
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
    wire [7:0] tri_red_byte_1 = tri_red_1[38:31];
    wire [7:0] tri_green_byte_1 = tri_green_1[38:31];
    wire [7:0] tri_blue_byte_1 = tri_blue_1[38:31];

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
            address <= 1'b0;
            read <= 1'b0;
            writedata <= 1'b0;
            write <= 1'b0;
        end else begin
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
                    address <= pc;
                    read <= 1'b1;
                    pc <= pc + 1'b1;
                    state <= STATE_WAIT_READ_COMMAND;
                end

                STATE_WAIT_READ_COMMAND: begin
                    // When no longer told to wait, deassert the request lines.
                    if (!waitrequest) begin
                        read <= 1'b0;
                    end

                    // If we have data, grab it.
                    if (readdatavalid) begin
                        command_word <= readdata;
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

                        default: begin
                            // Unhandled command, abort.
                            unhandled_count <= unhandled_count + 1'b1;
                            state <= STATE_INIT;
                        end
                    endcase
                end

                STATE_CMD_CLEAR: begin
                    address <= fb_address;
                    writedata <= {
                        8'b0,
                        command_word[47:40], // Blue
                        command_word[55:48], // Green
                        command_word[63:56], // Red
                        8'b0,
                        command_word[47:40], // Blue
                        command_word[55:48], // Green
                        command_word[63:56]  // Red
                    };
                    write <= 1;
                    byteenable <= 8'hFF;
                    state <= STATE_CMD_CLEAR_LOOP;
                end

                STATE_CMD_CLEAR_LOOP: begin
                    // Wait until we're not requested to wait.
                    if (!waitrequest) begin
                        if (address == fb_address + FB_LENGTH/8 - 1) begin
                            write <= 1'b0;
                            state <= STATE_READ_COMMAND;
                        end else begin
                            address <= address + 1'b1;
                        end
                    end
                end

                STATE_CMD_ZCLEAR: begin
                    address <= Z_ADDRESS/8;
                    writedata <= { z_clear_value, z_clear_value };
                    write <= 1;
                    byteenable <= 8'hFF;
                    state <= STATE_CMD_ZCLEAR_LOOP;
                end

                STATE_CMD_ZCLEAR_LOOP: begin
                    // Wait until we're not requested to wait.
                    if (!waitrequest) begin
                        if (address == Z_ADDRESS + FB_LENGTH/8 - 1) begin
                            write <= 1'b0;
                            state <= STATE_READ_COMMAND;
                        end else begin
                            address <= address + 1'b1;
                        end
                    end
                end

                STATE_CMD_PATTERN: begin
                    // Read first pattern word.
                    address <= pc;
                    read <= 1'b1;
                    pc <= pc + 1'b1;
                    state <= STATE_CMD_PATTERN_WAIT_READ_0;
                end

                STATE_CMD_PATTERN_WAIT_READ_0: begin
                    // When no longer told to wait, deassert the request lines.
                    if (!waitrequest) begin
                        read <= 1'b0;
                    end

                    // If we have data, grab it.
                    if (readdatavalid) begin
                        pattern[63:0] <= readdata;
                        address <= pc;
                        read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_PATTERN_WAIT_READ_1;
                    end
                end

                STATE_CMD_PATTERN_WAIT_READ_1: begin
                    // When no longer told to wait, deassert the request lines.
                    if (!waitrequest) begin
                        read <= 1'b0;
                    end

                    // If we have data, grab it.
                    if (readdatavalid) begin
                        pattern[127:64] <= readdata;
                        address <= pc;
                        read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_PATTERN_WAIT_READ_2;
                    end
                end

                STATE_CMD_PATTERN_WAIT_READ_2: begin
                    // When no longer told to wait, deassert the request lines.
                    if (!waitrequest) begin
                        read <= 1'b0;
                    end

                    // If we have data, grab it.
                    if (readdatavalid) begin
                        pattern[191:128] <= readdata;
                        address <= pc;
                        read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_PATTERN_WAIT_READ_3;
                    end
                end

                STATE_CMD_PATTERN_WAIT_READ_3: begin
                    // When no longer told to wait, deassert the request lines.
                    if (!waitrequest) begin
                        read <= 1'b0;
                    end

                    // If we have data, grab it.
                    if (readdatavalid) begin
                        pattern[255:192] <= readdata;
                        state <= STATE_READ_COMMAND;
                    end
                end

                STATE_CMD_DRAW: begin
                    // We only handle triangles, so no point in checking
                    // the type.
                    triangle_count <= command_word[31:16];
                    state <= STATE_CMD_DRAW_TRIANGLE_READ_0;
                end

                STATE_CMD_DRAW_TRIANGLE_READ_0: begin
                    // Turn off write, we may have left it on for
                    // the lower-right pixel of the last triangle's
                    // bounding box.
                    write <= 1'b0;

                    if (triangle_count == 1'b0) begin
                        // Done with all the triangles we had to draw.
                        state <= STATE_READ_COMMAND;
                    end else begin
                        triangle_count <= triangle_count - 1'b1;

                        // Read first vertex.
                        address <= pc;
                        read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_DRAW_TRIANGLE_WAIT_READ_0;
                    end
                end

                STATE_CMD_DRAW_TRIANGLE_WAIT_READ_0: begin
                    if (!waitrequest && !readdatavalid) begin
                        read <= 1'b0;
                    end

                    if (readdatavalid) begin
                        vertex_0 <= readdata;
                        address <= pc;
                        read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_DRAW_TRIANGLE_WAIT_READ_1;
                    end
                end

                STATE_CMD_DRAW_TRIANGLE_WAIT_READ_1: begin
                    if (!waitrequest && !readdatavalid) begin
                        read <= 1'b0;
                    end

                    if (readdatavalid) begin
                        vertex_1 <= readdata;
                        address <= pc;
                        read <= 1'b1;
                        pc <= pc + 1'b1;
                        state <= STATE_CMD_DRAW_TRIANGLE_WAIT_READ_2;
                    end
                end

                STATE_CMD_DRAW_TRIANGLE_WAIT_READ_2: begin
                    if (!waitrequest) begin
                        read <= 1'b0;
                    end

                    if (readdatavalid) begin
                        vertex_2 <= readdata;
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
                        tri_address_row <= upper_left_address;
                        tri_address <= upper_left_address;
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
                    state <= STATE_CMD_DRAW_TRIANGLE_PREPARE11;
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE11: begin
                    tri_red_0 <= tri_red_row;
                    tri_red_0 <= tri_red_row + tri_red_incr;
                    tri_green_0 <= tri_green_row;
                    tri_green_0 <= tri_green_row + tri_green_incr;
                    tri_blue_0 <= tri_blue_row;
                    tri_blue_0 <= tri_blue_row + tri_blue_incr;
                    state <= STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX_LOOP;
                end

                STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX_LOOP: begin
                    // Wait until we're not requested to wait.
                    if (!write || !waitrequest) begin
                        // Decide whether to draw this pixel.
                        write <= inside_triangle_0 || inside_triangle_1;
                        byteenable <= {{4{inside_triangle_1}},
                                       {4{inside_triangle_0}}};
                        address <= tri_address;
                        writedata <= {
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

                        // Advance to the next pixel.
                        if (tri_x_0 == tri_max_x) begin
                            if (tri_y == tri_max_y) begin
                                // Next triangle. We might still turn on
                                // write this clock, but we'll turn
                                // it off in the next state.
                                state <= STATE_CMD_DRAW_TRIANGLE_READ_0;
                            end else begin
                                // Next row.
                                tri_x_0 <= tri_min_x;
                                tri_x_1 <= tri_min_x + 1'b1;
                                tri_y <= tri_y + 1'b1;
                                tri_address <= tri_address_row;
                                tri_address_row <= tri_address_row + FB_WIDTH/2;
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
                                tri_red_row <= tri_red_row + tri_red_row_incr;
                                tri_green_row <= tri_green_row + tri_green_row_incr;
                                tri_blue_row <= tri_blue_row + tri_blue_row_incr;
                            end
                        end else begin
                            // Next pixel on this row.
                            tri_address <= tri_address + 1'b1;
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
                        end
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
