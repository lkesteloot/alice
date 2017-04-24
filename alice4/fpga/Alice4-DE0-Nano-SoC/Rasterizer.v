// Takes a stream of graphics commands and performs the drawing
// operations on the frame buffer.
module Rasterizer
    #(parameter FB_ADDRESS=0, FB_LENGTH=0, PROT_ADDRESS=0)
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

    // These get displayed on the LCD.
    output wire [31:0] debug_value0,
    output wire [31:0] debug_value1,
    output wire [31:0] debug_value2
);

    // Debugging.
    assign debug_value0 = {
        3'b0, waitrequest,
        3'b0, readdatavalid,
        8'b0,
        command,
        state
    };
    assign debug_value1 = pc;
    assign debug_value2 = address;

    // Constants.
    assign byteenable = 8'hFF;
    assign burstcount = 8'h01;

    // State machine.
    localparam STATE_INIT = 4'h0;
    localparam STATE_WAIT_FOR_DATA = 4'h1;
    localparam STATE_WAIT_FOR_NO_DATA = 4'h2;
    localparam STATE_READ_COMMAND = 4'h3;
    localparam STATE_WAIT_READ_COMMAND = 4'h4;
    localparam STATE_DECODE_COMMAND = 4'h5;
    localparam STATE_CMD_CLEAR = 4'h6;
    localparam STATE_CMD_CLEAR_LOOP = 4'h7;
    localparam STATE_CMD_SWAP = 4'h8;
    localparam STATE_CMD_END = 4'h9;
    reg [3:0] state;

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

    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            // State.
            state <= STATE_INIT;
            busy <= 1'b0;
            pc <= 1'b0;

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
                    address <= FB_ADDRESS/8;
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
                        if (address == FB_ADDRESS/8 + FB_LENGTH/8 - 1) begin
                            write <= 1'b0;
                            state <= STATE_READ_COMMAND;
                        end else begin
                            address <= address + 1'b1;
                        end
                    end
                end

                STATE_CMD_SWAP: begin
                    // Nothing to do currently, we have only one buffer.
                    state <= STATE_READ_COMMAND;
                end

                STATE_CMD_END: begin
                    state <= STATE_INIT;
                end
            endcase
        end
    end

endmodule
