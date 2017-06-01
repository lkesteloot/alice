// Test bench for Prescaler module.

`timescale 1ns/1ps

module Prescaler_tb();

    reg clock;
    reg reset_n;
    wire tick;

    Prescaler #(.SCALE(5), .SCALE_BITS(3)) prescaler(
        .clock(clock),
        .reset_n(reset_n),
        .tick(tick)
    );

    initial begin
        clock = 1'b0;
        reset_n = 1'b0;
        #1 reset_n = 1'b1;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b1;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b0;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b1;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b0;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b1;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b0;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b1;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b0;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b1;
        // Here tick has been set to 1, but we don't see it until the end of
        // the time period.
        if (tick != 1'b0) $error("tick has wrong value");
        #1
        if (tick != 1'b1) $error("tick has wrong value");
        clock = 1'b0;
        if (tick != 1'b1) $error("tick has wrong value");
        #1 clock = 1'b1;
        if (tick != 1'b1) $error("tick has wrong value");
        #1 clock = 1'b0;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b1;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b0;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b1;
        if (tick != 1'b0) $error("tick has wrong value");
        #1 clock = 1'b0;
        if (tick != 1'b0) $error("tick has wrong value");

        $finish;
    end

endmodule
