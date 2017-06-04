
// Debounces a button. The "changed" output is true for the first clock
// of the new value of "debounced_button". Takes 42 ms to settle.
module Debouncer #(parameter DEFAULT_VALUE=1'b0)
(
    input wire clock,
    input wire reset_n,
    input wire button,
    output reg debounced_button,
    output reg changed
);

    // 2**21 * 20 ns = 42 ms.
    localparam N = 21;

    reg [N-1:0] counter;

    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            debounced_button <= DEFAULT_VALUE;
            counter <= 1'b0;
            changed <= 1'b0;
        end else begin
            changed <= 1'b0;

            if (button == debounced_button) begin
                // Keep resetting our counter.
                counter <= {N{1'b1}};
            end else begin
                if (counter == 0) begin
                    // We've made it through the wait period, change
                    // our value.
                    debounced_button <= button;
                    counter <= {N{1'b1}};
                    changed <= 1'b1;
                end else begin
                    // Count down.
                    counter <= counter - 1'b1;
                end
            end
        end
    end

endmodule
