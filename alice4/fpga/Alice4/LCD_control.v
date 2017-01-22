// Driver for an LCD TFT display. Specifically, this was written to drive
// the AdaFruit YX700WV03, which is an 800x480 7" display. It's driven
// like a VGA monitor, but with digital color values, separate horizontal and
// vertical sync signals, a data enable signal, a clock, and with different
// timings.

module LCD_control(
    // Host Side
    input [7:0] iRed,
    input [7:0] iGreen,
    input [7:0] iBlue,
    output [9:0] oCurrent_X,    // Max horizontal pixels: 1023.
    output [9:0] oCurrent_Y,    // Max vertical pixels: 1023.
    output [21:0] oAddress,     // [0..(w*h))
    output oRequest,            // Whether we need a pixel right now.
    output reg oTopOfScreen,    // 1 when at the very top of screen.
    // LCD Side
    output [7:0] oLCD_R,
    output [7:0] oLCD_G,
    output [7:0] oLCD_B,
    output reg oLCD_HS,   // Active low.
    output reg oLCD_VS,   // Active low.
    output oLCD_DE,   // Active high.
    // Control Signal
    input iCLK,
    input iRST_N
);

// LK: There are two internal registers, H_Cont and V_Cont. They are 0-based. The
// H_Cont value has these ranges:
//
//    H_Cont                            oLCD_HS
//    [0, H_FRONT)                      1  (front porch)
//    [H_FRONT, H_FRONT + H_SYNC)       0  (sync pulse)
//    [H_FRONT + H_SYNC, H_BLANK)       1  (back porch, V_Cont is incremented)
//    [H_BLANK, H_TOTAL)                1  (pixels are visible)
//
// V_Cont value has these ranges:
//
//    V_Cont                            oLCD_VS
//    [0, V_FRONT)                      1  (front porch)
//    [V_FRONT, V_FRONT + V_SYNC)       0  (sync pulse)
//    [V_FRONT + V_SYNC, V_BLANK)       1  (back porch)
//    [V_BLANK, V_TOTAL)                1  (pixels are visible)
//
// Note that V_Cont is incremented on the positive edge of oLCD_HS, which means
// that its values are offset from the normal 0-799 range of H_Cont.
//
// oTopOfScreen is the first pixel of the second row, since that's where
// both are zero.
//
// The LCD clock is 25.175 MHz. With 992x500 pixels (800x480 visible),
// that's about 50 FPS.

//  Internal Registers
reg         [10:0]  H_Cont;
reg         [10:0]  V_Cont;
////////////////////////////////////////////////////////////
//  Horizontal  Parameter
parameter   H_FRONT =   24;
parameter   H_SYNC  =   72;
parameter   H_BACK  =   96;
parameter   H_ACT   =   800;
parameter   H_BLANK =   H_FRONT+H_SYNC+H_BACK;
parameter   H_TOTAL =   H_FRONT+H_SYNC+H_BACK+H_ACT;
////////////////////////////////////////////////////////////
//  Vertical Parameter
parameter   V_FRONT =   3;
parameter   V_SYNC  =   10;
parameter   V_BACK  =   7;
parameter   V_ACT   =   480;
parameter   V_BLANK =   V_FRONT+V_SYNC+V_BACK;
parameter   V_TOTAL =   V_FRONT+V_SYNC+V_BACK+V_ACT;
////////////////////////////////////////////////////////////
assign  oLCD_R      =   oRequest ? iRed : 8'b0 ;
assign  oLCD_G      =   oRequest ? iGreen : 8'b0 ;
assign  oLCD_B      =   oRequest ? iBlue : 8'b0 ;
assign  oAddress    =   oCurrent_Y*H_ACT + oCurrent_X;
assign  oRequest    =   H_Cont >= H_BLANK && V_Cont >= V_BLANK;
assign  oCurrent_X  =   (H_Cont>=H_BLANK) ? H_Cont-H_BLANK : 11'h0;
assign  oCurrent_Y  =   (V_Cont>=V_BLANK) ? V_Cont-V_BLANK : 11'h0;
assign  oLCD_DE     =   H_Cont>=H_BLANK;


// Latch the top-of-screen register. That means that it's one clock late,
// but that's okay since it's nowhere near the visible portion of the screen.
wire oTopOfScreenNext = H_Cont == 0 && V_Cont == 0;
always @(posedge iCLK)
begin
    oTopOfScreen <= oTopOfScreenNext;
end

// Horizontal Generator: Refers to the pixel clock.
always @(posedge iCLK or negedge iRST_N)
begin
    if (!iRST_N) begin
        H_Cont <= 0;
        oLCD_HS <= 1;
    end else begin
        // Advance pixel.
        if (H_Cont < H_TOTAL - 1) begin
            H_Cont <= H_Cont + 1'b1;
        end else begin
            H_Cont <= 0;
        end

        // Horizontal Sync
        if (H_Cont == H_FRONT - 1) begin
            // Front porch end
            oLCD_HS <= 1'b0;
        end
        if (H_Cont == H_FRONT + H_SYNC - 1) begin
            // Sync pulse end
            oLCD_HS <= 1'b1;
        end
    end
end

// Vertical Generator: Refers to the horizontal sync.
always @(posedge iCLK or negedge iRST_N)
begin
    if (!iRST_N) begin
        V_Cont <= 0;
        oLCD_VS <= 1;
    end else if (H_Cont == 0) begin
        // Advance line.
        if (V_Cont < V_TOTAL-1) begin
            V_Cont <= V_Cont+1'b1;
        end else begin
            V_Cont <= 0;
        end

        // Vertical Sync
        if (V_Cont == V_FRONT - 1) begin
            // Front porch end
            oLCD_VS <= 1'b0;
        end
        if (V_Cont == V_FRONT + V_SYNC - 1) begin
            // Sync pulse end
            oLCD_VS <= 1'b1;
        end
    end
end

endmodule
