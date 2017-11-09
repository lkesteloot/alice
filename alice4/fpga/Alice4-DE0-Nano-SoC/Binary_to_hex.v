
// Converts a 4-bit binary integer to an ASCII hex uppercase digit.
module Binary_to_hex(
    input wire [3:0] value,
    output reg [6:0] hex
);

always @(*) begin
    case (value)
        4'h0: hex = 7'h30;
        4'h1: hex = 7'h31;
        4'h2: hex = 7'h32;
        4'h3: hex = 7'h33;
        4'h4: hex = 7'h34;
        4'h5: hex = 7'h35;
        4'h6: hex = 7'h36;
        4'h7: hex = 7'h37;
        4'h8: hex = 7'h38;
        4'h9: hex = 7'h39;
        4'ha: hex = 7'h41;
        4'hb: hex = 7'h42;
        4'hc: hex = 7'h43;
        4'hd: hex = 7'h44;
        4'he: hex = 7'h45;
        default: hex = 7'h46;
    endcase
end

endmodule
