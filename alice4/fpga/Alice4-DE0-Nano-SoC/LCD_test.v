
// Generate a test pattern.

module LCD_test(
    input wire [6:0] column,
    input wire [5:0] row,
    output wire [6:0] character
);

// Figure out what character to draw.
wire [12:0] address = { row, column };
assign character = address[6:0] ^ (row[0] ? 7'h40 : 7'h00);

endmodule
