
// Generate a debug screen, displaying the hex value of three 32-bit registers.

module LCD_debug(
    input wire [6:0] column,
    input wire [5:0] row,
    input wire [31:0] value0,
    input wire [31:0] value1,
    input wire [31:0] value2,
    output reg [6:0] character
);

// Which value to display.
reg [31:0] display;

// Binary-to-hex converter.
reg [3:0] binary;
wire [6:0] hex;
Binary_to_hex binary_to_hex(
    .value(binary),
    .hex(hex)
);

// Lay out screen.
reg show_hex;
always @(*) begin
    // Configure value by row.
    case (row)
        6'd2: display = value0;
        6'd3: display = value1;
        default: display = value2;
    endcase
end

always @(*) begin
    // Configure nybble for value output.
    case (column)
        7'd0: { show_hex, binary } = { 1'b1, display[31:28] };
        7'd1: { show_hex, binary } = { 1'b1, display[27:24] };
        7'd2: { show_hex, binary } = { 1'b1, display[23:20] };
        7'd3: { show_hex, binary } = { 1'b1, display[19:16] };
        7'd5: { show_hex, binary } = { 1'b1, display[15:12] };
        7'd6: { show_hex, binary } = { 1'b1, display[11:8] };
        7'd7: { show_hex, binary } = { 1'b1, display[7:4] };
        7'd8: { show_hex, binary } = { 1'b1, display[3:0] };
        default: { show_hex, binary } = { 1'b0, 4'h0 };
    endcase
end

always @(*) begin
    // Select output.
    if (row == 6'd0) begin
        case (column)
            7'd0: character = 7'h41;
            7'd1: character = 7'h6C;
            7'd2: character = 7'h69;
            7'd3: character = 7'h63;
            7'd4: character = 7'h65;
            7'd6: character = 7'h34;
            7'd8: character = 7'h03;
            default: character = 7'h20;
        endcase
    end else if (row >= 6'd2 && row <= 6'd4) begin
        character = show_hex ? hex : 7'h20;
    end else begin
        character = 7'h20;
    end
end

endmodule
