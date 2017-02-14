
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

    assign red = checkerboard ? 8'hff : 8'h00;
    assign green = checkerboard ? 8'hff : 8'h00;
    assign blue = checkerboard ? 8'hff : 8'h00;

endmodule
