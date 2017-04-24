// Takes a stream of graphics commands and performs the drawing
// operations on the frame buffer.
module Rasterizer
    // In bytes.
    #(parameter FB_ADDRESS=0, FB_LENGTH=0, FB_WIDTH=0, PROT_ADDRESS=0)
(
    // Clock and reset.
    input clock,
    input wire reset_n,

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
    output wire [7:0] byteenable,
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
        6'b0,
        tri_min_x,
        7'b0,
        tri_min_y
    };
    assign debug_value1 = pc;
    assign debug_value2 = address;

    // Constants.
    assign byteenable = 8'hFF;
    assign burstcount = 8'h01;

    // State machine.
    localparam STATE_INIT = 5'h00;
    localparam STATE_WAIT_FOR_DATA = 5'h01;
    localparam STATE_WAIT_FOR_NO_DATA = 5'h02;
    localparam STATE_READ_COMMAND = 5'h03;
    localparam STATE_WAIT_READ_COMMAND = 5'h04;
    localparam STATE_DECODE_COMMAND = 5'h05;
    localparam STATE_CMD_CLEAR = 5'h06;
    localparam STATE_CMD_CLEAR_LOOP = 5'h07;
    localparam STATE_CMD_DRAW = 5'h08;
    localparam STATE_CMD_DRAW_TRIANGLE_READ_0 = 5'h09;
    localparam STATE_CMD_DRAW_TRIANGLE_WAIT_READ_0 = 5'h0A;
    localparam STATE_CMD_DRAW_TRIANGLE_WAIT_READ_1 = 5'h0B;
    localparam STATE_CMD_DRAW_TRIANGLE_WAIT_READ_2 = 5'h0C;
    localparam STATE_CMD_DRAW_TRIANGLE_PREPARE = 5'h0D;
    localparam STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX = 5'h0E;
    localparam STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX_LOOP = 5'h0F;
    localparam STATE_CMD_SWAP = 5'h1D;
    localparam STATE_CMD_SWAP_WAIT = 5'h1E;
    localparam STATE_CMD_END = 5'h1F;
    reg [4:0] state;

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

    // The most recent command word we've read.
    reg [63:0] command_word;
    wire [7:0] command = command_word[7:0];

    // Drawing state.
    reg [15:0] triangle_count;
    reg [63:0] vertex_0;
    reg [63:0] vertex_1;
    reg [63:0] vertex_2;
    wire [9:0] vertex_0_x = vertex_0[11:2];
    wire [8:0] vertex_0_y = vertex_0[23:15];
    wire [9:0] vertex_1_x = vertex_1[11:2];
    wire [8:0] vertex_1_y = vertex_1[23:15];
    wire [9:0] vertex_2_x = vertex_2[11:2];
    wire [8:0] vertex_2_y = vertex_2[23:15];
    reg [9:0] tri_x;
    reg [8:0] tri_y;
    reg [9:0] tri_min_x;
    reg [8:0] tri_min_y;
    reg [9:0] tri_max_x;
    reg [8:0] tri_max_y;
    reg [28:0] tri_left_address;

    // We draw to the back buffer.
    wire [28:0] fb_address = rast_front_buffer ? FB_ADDRESS/8 : (FB_ADDRESS + FB_LENGTH)/8;

    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            // State.
            state <= STATE_INIT;
            busy <= 1'b0;
            pc <= 1'b0;
            triangle_count <= 1'b0;
            tri_x <= 1'b0;
            tri_y <= 1'b0;
            tri_min_x <= 1'b0;
            tri_min_y <= 1'b0;
            tri_max_x <= 1'b0;
            tri_max_y <= 1'b0;
            rast_front_buffer <= 1'b0;

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
                        pc <= PROT_ADDRESS/8;
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
                            // Unhandled command. Skip it and hope there's
                            // no additional data.
                            state <= STATE_READ_COMMAND;
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

                STATE_CMD_DRAW: begin
                    // We only handle triangles, so no point in checking
                    // the type.
                    triangle_count <= command_word[31:16];
                    state <= STATE_CMD_DRAW_TRIANGLE_READ_0;
                end

                STATE_CMD_DRAW_TRIANGLE_READ_0: begin
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
                        state <= STATE_CMD_DRAW_TRIANGLE_PREPARE;
                    end
                end

                STATE_CMD_DRAW_TRIANGLE_PREPARE: begin
                    // Compute bounding box.
                    tri_min_x <= (vertex_0_x < vertex_1_x)
                                    ? (vertex_0_x < vertex_2_x)
                                        ? vertex_0_x 
                                        : vertex_2_x
                                    : (vertex_1_x < vertex_2_x)
                                        ? vertex_1_x
                                        : vertex_2_x;
                    tri_min_y <= (vertex_0_y < vertex_1_y)
                                    ? (vertex_0_y < vertex_2_y)
                                        ? vertex_0_y 
                                        : vertex_2_y
                                    : (vertex_1_y < vertex_2_y)
                                        ? vertex_1_y
                                        : vertex_2_y;
                    tri_max_x <= (vertex_0_x > vertex_1_x)
                                    ? (vertex_0_x > vertex_2_x)
                                        ? vertex_0_x 
                                        : vertex_2_x
                                    : (vertex_1_x > vertex_2_x)
                                        ? vertex_1_x
                                        : vertex_2_x;
                    tri_max_y <= (vertex_0_y > vertex_1_y)
                                    ? (vertex_0_y > vertex_2_y)
                                        ? vertex_0_y 
                                        : vertex_2_y
                                    : (vertex_1_y > vertex_2_y)
                                        ? vertex_1_y
                                        : vertex_2_y;
                    state <= STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX;
                end

                STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX: begin
                    tri_x <= tri_min_x;
                    tri_y <= tri_min_y;
                    tri_left_address <= fb_address + (tri_min_y*FB_WIDTH + tri_min_x)/2;
                    writedata <= 64'h0000FF00_0000FF00;
                    address <= fb_address + (tri_min_y*FB_WIDTH + tri_min_x)/2;
                    write <= 1'b1;
                    state <= STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX_LOOP;
                end

                STATE_CMD_DRAW_TRIANGLE_DRAW_BBOX_LOOP: begin
                    // Wait until we're not requested to wait.
                    if (!waitrequest) begin
                        if (tri_x >= tri_max_x) begin
                            if (tri_y == tri_max_y) begin
                                write <= 1'b0;
                                // Next triangle.
                                state <= STATE_CMD_DRAW_TRIANGLE_READ_0;
                            end else begin
                                tri_x <= tri_min_x;
                                tri_y <= tri_y + 1'b1;
                                address <= tri_left_address;
                                tri_left_address <= tri_left_address + FB_WIDTH/2;
                            end
                        end else begin
                            address <= address + 1'b1;
                            tri_x <= tri_x + 2'd2;
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
            endcase
        end
    end

endmodule
