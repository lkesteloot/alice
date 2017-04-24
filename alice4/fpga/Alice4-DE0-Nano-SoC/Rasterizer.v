module Rasterizer(
    // Clock and reset.
    input clock,
    input wire reset_n,

    // Semaphores to data-producing module.
    input data_ready,
    output reg busy
);

    // State machine.
    localparam STATE_INIT = 4'h0;
    localparam STATE_WAIT_FOR_DATA = 4'h1;
    localparam STATE_WAIT_FOR_NO_DATA = 4'h2;
    localparam STATE_RASTERIZE = 4'h3;
    reg [3:0] state;

    reg [26:0] counter;

    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            state <= STATE_INIT;
            busy <= 1'b0;
            counter <= 1'b0;
        end else begin
            case (state)
                STATE_INIT: begin
                    busy <= 1'b0;
                    state <= STATE_WAIT_FOR_DATA;
                end

                STATE_WAIT_FOR_DATA: begin
                    if (data_ready) begin
                        state <= STATE_WAIT_FOR_NO_DATA;
                        busy <= 1'b1;
                    end
                end

                STATE_WAIT_FOR_NO_DATA: begin
                    if (!data_ready) begin
                        state <= STATE_RASTERIZE;
                        counter <= 1'b0;
                    end
                end

                STATE_RASTERIZE: begin
                    if (counter == 26'd50_000_000) begin
                        state <= STATE_INIT;
                    end else begin
                        counter <= counter + 1'b1;
                    end
                end
            endcase
        end
    end

endmodule
