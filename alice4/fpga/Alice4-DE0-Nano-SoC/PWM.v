// Module that generates PWM.
module PWM
    #(parameter MAX_VALUE=1000,BITS=10)
(
    input clock,
    input reset_n,
    input tick,
    input [BITS-1:0] value,
    output reg signal
);

    reg [BITS-1:0] counter;

    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            counter <= 1'b0;
            signal <= 1'b0;
        end else begin
            if (tick) begin
                // Stop one early so that we can be all-on.
                if (counter == MAX_VALUE - 1) begin
                    counter <= 1'b0;
                end else begin
                    counter <= counter + 1'b1;
                end
                signal <= counter < value;
            end
        end
    end

endmodule
