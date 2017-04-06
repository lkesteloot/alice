// Driver for an LCD TFT display. Specifically, this was written to drive
// the AdaFruit YX700WV03, which is an 800x480 7" display. It's driven
// like a VGA monitor, but with digital color values, a data enable signal,
// and a clock.

module LCD_control(
    input wire clock,                // System clock.
    input wire tick,                 // LCD clock (synchronous with system clock).
    input wire reset_n,              // Asynchronous reset, active low.
    output reg [9:0] x,              // On-screen X pixel location.
    output reg [9:0] y,              // On-screen Y pixel location.
    output reg next_frame,           // 1 when between frames.
    output reg hs_n,                 // Horizontal sync, active low.
    output reg vs_n,                 // Vertical sync, active low.
    output reg data_enable           // Whether we need a pixel right now.
);

    // There are two internal registers, h and v. They are 0-based. The
    // h value has these ranges:
    //
    //    h                                 hs_n
    //    [0, H_FRONT)                      1  (front porch)
    //    [H_FRONT, H_FRONT + H_SYNC)       0  (sync pulse)
    //    [H_FRONT + H_SYNC, H_BLANK)       1  (back porch, v is incremented)
    //    [H_BLANK, H_TOTAL)                1  (pixels are visible)
    //
    // v value has these ranges:
    //
    //    v                                 vs_n
    //    [0, V_FRONT)                      1  (front porch)
    //    [V_FRONT, V_FRONT + V_SYNC)       0  (sync pulse)
    //    [V_FRONT + V_SYNC, V_BLANK)       1  (back porch)
    //    [V_BLANK, V_TOTAL)                1  (pixels are visible)
    //
    // Note that v is incremented on the positive edge of hs_n, which means
    // that its values are offset from the normal 0-799 range of h.
    //
    // next_frame is the first pixel of the second row, since that's where
    // both are zero.

    // Video parameters.
    parameter H_FRONT = 24;
    parameter H_SYNC  = 72;
    parameter H_BACK  = 96;
    parameter H_ACT   = 800;
    parameter H_BLANK = H_FRONT + H_SYNC + H_BACK;
    parameter H_TOTAL = H_FRONT + H_SYNC + H_BACK + H_ACT;

    parameter V_FRONT = 3;
    parameter V_SYNC  = 10;
    parameter V_BACK  = 7;
    parameter V_ACT   = 480;
    parameter V_BLANK = V_FRONT + V_SYNC + V_BACK;
    parameter V_TOTAL = V_FRONT + V_SYNC + V_BACK + V_ACT;

    reg [10:0] h;
    reg [10:0] v;
    wire h_visible = h >= H_BLANK;
    wire v_visible = v >= V_BLANK;

    // Latch the next_frame register. This will be true two pixels after the 
    // last visible pixel.
    always @(posedge clock) begin
        if (tick) begin
            next_frame <= h == 0 && v == 0;
        end
    end

    // Walk through screen.
    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            h <= 0;
            v <= 0;
            hs_n <= 1;
            vs_n <= 1;
            x <= 0;
            y <= 0;
            data_enable <= 0;
        end else if (tick) begin
            // Advance pixel.
            if (h < H_TOTAL - 1) begin
                h <= h + 1'b1;
            end else begin
                // Next line.
                h <= 0;

                if (v < V_TOTAL-1) begin
                    v <= v + 1'b1;
                end else begin
                    v <= 0;
                end

                // Vertical sync.
                if (v == V_FRONT - 1) begin
                    // Front porch end.
                    vs_n <= 1'b0;
                end
                if (v == V_FRONT + V_SYNC - 1) begin
                    // Sync pulse end.
                    vs_n <= 1'b1;
                end
            end

            // Horizontal sync.
            if (h == H_FRONT - 1) begin
                // Front porch end.
                hs_n <= 1'b0;
            end
            if (h == H_FRONT + H_SYNC - 1) begin
                // Sync pulse end.
                hs_n <= 1'b1;
            end
            
            // Latch output registers.
            x <= h_visible ? h - H_BLANK : 10'h0;
            y <= v_visible ? v - V_BLANK : 10'h0;
            data_enable <= h_visible && v_visible;
        end
    end

endmodule
