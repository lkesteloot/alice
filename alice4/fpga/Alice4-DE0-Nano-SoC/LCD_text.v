// Given a pixel location, determines the text character position.
module LCD_text
    // LOG2_SCALE=0 for no scaling, 1 for double, 2 for quadruple, etc.
    #(parameter LOG2_SCALE=0)
(
    input wire [9:0] x,
    input wire [9:0] y,
    output wire [6:0] column,
    output wire [5:0] row,
    output wire [2:0] character_column,
    output wire [3:0] character_row
);

// Scale the screen.
wire [9:0] scaled_x = x >>> LOG2_SCALE;
wire [9:0] scaled_y = y >>> LOG2_SCALE;

// Break screen down into characters.
assign column = scaled_x[9:3];
assign row = scaled_y[9:4];
assign character_column = scaled_x[2:0];
assign character_row = scaled_y[3:0];

endmodule
