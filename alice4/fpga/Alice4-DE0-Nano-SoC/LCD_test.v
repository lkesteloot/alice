
// Generate a test pattern.

module LCD_test(
    input wire clock,
    input wire reset_n,
    input wire [6:0] column,
    input wire [5:0] row,
    output wire [6:0] character
);

/*
// Figure out what character to draw.
wire [12:0] address = { row, column };
assign character = address[6:0] ^ (row[0] ? 7'h40 : 7'h00);
*/

reg [25:0] counter;
reg [15:0] display;

always @(posedge clock or negedge reset_n) begin
    if (!reset_n) begin
        counter <= 1'b0;
        display <= 16'h0000;
    end else begin
        if (counter == 49_000_000) begin
            counter <= 0;
            display <= display + 1'b1;
        end else begin
            counter <= counter + 1'b1;
        end
    end
end

reg [3:0] binary;
wire [6:0] hex;
Binary_to_hex binary_to_hex(
    .value(binary),
    .hex(hex)
);
assign character = (row == 0 && column <= 3) ? hex : 7'h20;

always @(*) begin
    case (column)
        7'd0: binary <= display[15:12];
        7'd1: binary <= display[11:8];
        7'd2: binary <= display[7:4];
        7'd3: binary <= display[3:0];
        default: binary <= 4'h0;
    endcase
end

endmodule
