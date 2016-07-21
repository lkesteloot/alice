module Vga_control(
    //  Host Side
    input [3:0] iRed,
    input [3:0] iGreen,
    input [3:0] iBlue,
    output [9:0] oCurrent_X, // Max horizontal pixels: 1023.
    output [9:0] oCurrent_Y, // Max vertical pixels: 1023.
    output [21:0] oAddress,
    output oRequest,
	 output oTopOfScreen,     // 1 when at the very top of (blank) screen.
    //  VGA Side
    output [3:0] oVGA_R,
    output [3:0] oVGA_G,
    output [3:0] oVGA_B,
    output reg oVGA_HS,   // Active low.
    output reg oVGA_VS,   // Active low.
    output oVGA_BLANK,    // Active low.
    output oVGA_CLOCK,    // Inverse of iCLK.
    //  Control Signal
    input iCLK,
    input iRST_N
);

// LK: There are two internal registers, H_Cont and V_Cont. They are 0-based. The
// H_Cont value has these ranges:
//
//    H_Cont                            oVGA_HS
//    [0, H_FRONT)                      1
//    [H_FRONT, H_FRONT + H_SYNC)       0
//    [H_FRONT + H_SYNC, H_BLANK)       1  (V_Cont is incremented)
//    [H_BLANK, H_TOTAL)                1  (pixels are visible)
//
// V_Cont value has these ranges:
//
//    V_Cont                            oVGA_VS
//    [0, V_FRONT)                      1
//    [V_FRONT, V_FRONT + V_SYNC)       0
//    [V_FRONT + V_SYNC, V_BLANK)       1
//    [V_BLANK, V_TOTAL)                1  (pixels are visible)
//
// Note that V_Cont is incremented on the positive edge of oVGA_HS, which means
// that its values are offset from the normal 0-639 range of H_Cont.
// oTopOfScreen is the first pixel of the second row, since that's where
// both are zero.
//
// The VGA clock is 25.175 MHz. With 800x525 pixels (640x480 visible),
// that's 59.94 FPS.

//  Internal Registers
reg         [10:0]  H_Cont;
reg         [10:0]  V_Cont;
////////////////////////////////////////////////////////////
//  Horizontal  Parameter
parameter   H_FRONT =   16;
parameter   H_SYNC  =   96;
parameter   H_BACK  =   48;
parameter   H_ACT   =   640;
parameter   H_BLANK =   H_FRONT+H_SYNC+H_BACK;
parameter   H_TOTAL =   H_FRONT+H_SYNC+H_BACK+H_ACT;
////////////////////////////////////////////////////////////
//  Vertical Parameter
parameter   V_FRONT =   10;
parameter   V_SYNC  =   2;
parameter   V_BACK  =   33;
parameter   V_ACT   =   480;
parameter   V_BLANK =   V_FRONT+V_SYNC+V_BACK;
parameter   V_TOTAL =   V_FRONT+V_SYNC+V_BACK+V_ACT;
////////////////////////////////////////////////////////////
assign  oVGA_BLANK  =   ~(H_Cont<H_BLANK || V_Cont<V_BLANK);
assign  oVGA_CLOCK  =   ~iCLK;
//assign  oVGA_R      =   oRequest ? iRed : 4'b0 ;
//assign  oVGA_G      =   oRequest ? iGreen : 4'b0 ;
//assign  oVGA_B      =   oRequest ? iBlue : 4'b0 ;
assign  oVGA_R      =   (oCurrent_X > 10 && oCurrent_X < 630) ? iRed : 4'b0 ;
assign  oVGA_G      =   (oCurrent_X > 10 && oCurrent_X < 630) ? iGreen : 4'b0 ;
assign  oVGA_B      =   (oCurrent_X > 10 && oCurrent_X < 630) ? iBlue : 4'b0 ;
assign  oAddress    =   oCurrent_Y*H_ACT + oCurrent_X;
assign  oRequest    =   H_Cont >= H_BLANK && V_Cont >= V_BLANK;
assign  oCurrent_X  =   (H_Cont>=H_BLANK) ? H_Cont-H_BLANK : 11'h0;
assign  oCurrent_Y  =   (V_Cont>=V_BLANK) ? V_Cont-V_BLANK : 11'h0;
assign  oTopOfScreen = H_Cont == 0 && V_Cont == 0;

//  Horizontal Generator: Refer to the pixel clock
always@(posedge iCLK or negedge iRST_N)
begin
    if(!iRST_N)
    begin
        H_Cont      <=  0;
        oVGA_HS     <=  1;
    end
    else
    begin
        if(H_Cont<H_TOTAL-1)
        H_Cont  <=  H_Cont+1'b1;
        else
        H_Cont  <=  0;
        //  Horizontal Sync
        if(H_Cont==H_FRONT-1)           //  Front porch end
        oVGA_HS <=  1'b0;
        if(H_Cont==H_FRONT+H_SYNC-1)    //  Sync pulse end
        oVGA_HS <=  1'b1;
    end
end

//  Vertical Generator: Refer to the horizontal sync
always@(posedge oVGA_HS or negedge iRST_N)
begin
    if(!iRST_N)
    begin
        V_Cont      <=  0;
        oVGA_VS     <=  1;
    end
    else
    begin
        if(V_Cont<V_TOTAL-1)
        V_Cont  <=  V_Cont+1'b1;
        else
        V_Cont  <=  0;
        //  Vertical Sync
        if(V_Cont==V_FRONT-1)           //  Front porch end
        oVGA_VS <=  1'b0;
        if(V_Cont==V_FRONT+V_SYNC-1)    //  Sync pulse end
        oVGA_VS <=  1'b1;
    end
end

endmodule
