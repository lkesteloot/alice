
// Given a character and a position within it, returns a black and white pixel.
// The output is delayed by one clock cycle, and is not registered.
module LCD_font(
    input wire clock,
    input wire [6:0] character,
    input wire [2:0] character_column,
    input wire [3:0] character_row,
    output wire bw
);

// Look up character row byte.
wire [10:0] font_addr = { character, character_row };
wire [7:0] font_data;
Font_ROM font_ROM(clock, font_addr, font_data);

// The ~ trick is to reverse the order of the bits in font_data.
assign bw = font_data[~character_column];

endmodule
