
// Generate a test pattern.

module LCD_test(
    input clock,
    input [9:0] x,
    input [9:0] y,
    output [7:0] red,
    output [7:0] green,
    output [7:0] blue
);

wire checkerboard = x[4] ^ y[4];
reg bw;
wire [7:0] gray = bw ? 8'h80 : 8'h00;
assign red = gray;
assign green = gray;
assign blue = gray;

// Break screen down into characters.
wire [5:0] row = y[9:4];
wire [6:0] column = x[9:3];
wire [3:0] character_row = y[3:0];
wire [2:0] character_column = x[2:0];

// Figure out what character to draw.
wire [12:0] address = { row, column };
wire [6:0] character = address[6:0] ^ (row[0] ? 7'h40 : 7'h00);

// Look up character and bit.
wire [10:0] font_addr = { character, character_row };
wire [7:0] font_data;
Font_ROM fontROM(clock, font_addr, font_data);

always @(*) begin
    if (0) begin
        bw <= checkerboard;
    end else begin
        // The ~ trick is to reverse the order of the bits in font_data.
        bw <= font_data[~character_column];
    end
end

endmodule
