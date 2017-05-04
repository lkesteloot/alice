// Generates a tick every SCALE clocks.

module Prescaler
    #(parameter SCALE=1000,SCALE_BITS=10)
(
    input clock,
    input reset_n,
    output reg tick
);

    reg [SCALE_BITS-1:0] counter;

    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            counter <= 1'b0;
            tick <= 1'b0;
        end else begin
            if (counter == SCALE - 1) begin
                counter <= 1'b0;
                tick <= 1'b1;
            end else begin
                counter <= counter + 1'b1;
                tick <= 1'b0;
            end
        end
    end

endmodule
