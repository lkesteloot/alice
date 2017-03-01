
// Generate a test pattern.

module LCD_test(
    input wire clock,
    input wire [9:0] x,
    input wire [9:0] y,
    output wire [7:0] red,
    output wire [7:0] green,
    output wire [7:0] blue
);

wire checkerboard = x[4] ^ y[4];

// Color assignment.
reg bw;
wire [7:0] gray = bw ? 8'h80 : 8'h00;
assign red = gray;
assign green = gray;
assign blue = gray;

// Scale the screen.
wire [9:0] scaled_x = x >>> 1;
wire [9:0] scaled_y = y >>> 1;

// Break screen down into characters.
wire [5:0] row = scaled_y[9:4];
wire [6:0] column = scaled_x[9:3];
wire [3:0] character_row = scaled_y[3:0];
wire [2:0] character_column = scaled_x[2:0];

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
