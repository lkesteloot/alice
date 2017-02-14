// --------------------------------------------------------------------
// Copyright (c) 2009 by Terasic Technologies Inc.
// --------------------------------------------------------------------
//
// Permission:
//
//   Terasic grants permission to use and modify this code for use
//   in synthesis for all Terasic Development Boards and Altera Development
//   Kits made by Terasic.  Other use of this code, including the selling
//   ,duplication, or modification of any portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design reference
//   which illustrates how these types of functions can be implemented.
//   It is the user's responsibility to verify their design for
//   consistency and functionality through the use of formal
//   verification methods.  Terasic provides no warranty regarding the use
//   or functionality of this code.
//
// --------------------------------------------------------------------
//
//                     Terasic Technologies Inc
//                     356 Fu-Shin E. Rd Sec. 1. JhuBei City,
//                     HsinChu County, Taiwan
//                     302
//
//                     web: http://www.terasic.com/
//                     email: support@terasic.com
//
// --------------------------------------------------------------------
//
// Major Functions: DE0 TOP
//
// --------------------------------------------------------------------
//
// Revision History :
// --------------------------------------------------------------------
//   Ver  :| Author            :| Mod. Date :| Changes Made:

// --------------------------------------------------------------------


module Alice4
    (
        ////////////////////////    Clock Input     ////////////////////////
        input           CLOCK_50,               //  50 MHz
        input           CLOCK_50_2,             //  50 MHz
        ////////////////////////    Push Button     ////////////////////////
        input   [2:0]   BUTTON,                 //  Pushbutton[2:0]
        ////////////////////////    DPDT Switch     ////////////////////////
        // input   [9:0]   SW,                     //  Toggle Switch[9:0]
        ////////////////////////    7-SEG Dispaly   ////////////////////////
        output  [6:0]   HEX0_D,                 //  Seven Segment Digit 0
        output          HEX0_DP,                //  Seven Segment Digit DP 0
        output  [6:0]   HEX1_D,                 //  Seven Segment Digit 1
        output          HEX1_DP,                //  Seven Segment Digit DP 1
        output  [6:0]   HEX2_D,                 //  Seven Segment Digit 2
        output          HEX2_DP,                //  Seven Segment Digit DP 2
        output  [6:0]   HEX3_D,                 //  Seven Segment Digit 3
        output          HEX3_DP,                //  Seven Segment Digit DP 3
        ////////////////////////////    LED     ////////////////////////////
        output  [9:0]   LEDG,                   //  LED Green[9:0]
        ////////////////////////////    UART    ////////////////////////////
        // output          UART_TXD,               //  UART Transmitter
        // input           UART_RXD,               //  UART Receiver
        // output          UART_CTS,               //  UART Clear To Send
        // input           UART_RTS,               //  UART Request To Send
        ///////////////////////     SDRAM Interface ////////////////////////
        inout   [15:0]  DRAM_DQ,                //  SDRAM Data bus 16 Bits
        output  [12:0]  DRAM_ADDR,              //  SDRAM Address bus 13 Bits
        output          DRAM_LDQM,              //  SDRAM Low-byte Data Mask
        output          DRAM_UDQM,              //  SDRAM High-byte Data Mask
        output          DRAM_WE_N,              //  SDRAM Write Enable
        output          DRAM_CAS_N,             //  SDRAM Column Address Strobe
        output          DRAM_RAS_N,             //  SDRAM Row Address Strobe
        output          DRAM_CS_N,              //  SDRAM Chip Select
        output          DRAM_BA_0,              //  SDRAM Bank Address 0
        output          DRAM_BA_1,              //  SDRAM Bank Address 1
        output          DRAM_CLK,               //  SDRAM Clock
        output          DRAM_CKE,               //  SDRAM Clock Enable
        ////////////////////////    Flash Interface ////////////////////////
        // inout   [14:0]  FL_DQ,                  //  FLASH Data bus 15 Bits
        // inout           FL_DQ15_AM1,            //  FLASH Data bus Bit 15 or Address A-1
        // output  [21:0]  FL_ADDR,                //  FLASH Address bus 22 Bits
        // output          FL_WE_N,                //  FLASH Write Enable
        // output          FL_RST_N,               //  FLASH Reset
        // output          FL_OE_N,                //  FLASH Output Enable
        // output          FL_CE_N,                //  FLASH Chip Enable
        // output          FL_WP_N,                //  FLASH Hardware Write Protect
        // output          FL_BYTE_N,              //  FLASH Selects 8/16-bit mode
        // input           FL_RY,                  //  FLASH Ready/Busy
        ////////////////////    LCD Module 16X2 ////////////////////////////
        // inout   [7:0]   LCD_DATA,               //  LCD Data bus 8 bits
        // output          LCD_BLON,               //  LCD Back Light ON/OFF
        // output          LCD_RW,                 //  LCD Read/Write Select, 0 = Write, 1 = Read
        // output          LCD_EN,                 //  LCD Enable
        // output          LCD_RS,                 //  LCD Command/Data Select, 0 = Command, 1 = Data
        ////////////////////    SD Card Interface   ////////////////////////
        // inout           SD_DAT0,                //  SD Card Data 0
        // inout           SD_DAT3,                //  SD Card Data 3
        // inout           SD_CMD,                 //  SD Card Command Signal
        // output          SD_CLK,                 //  SD Card Clock
        // input           SD_WP_N,                //  SD Card Write Protect
        ////////////////////////    PS2     ////////////////////////////////
        // inout           PS2_KBDAT,              //  PS2 Keyboard Data
        // inout           PS2_KBCLK,              //  PS2 Keyboard Clock
        // inout           PS2_MSDAT,              //  PS2 Mouse Data
        // inout           PS2_MSCLK,              //  PS2 Mouse Clock
        ////////////////////////    VGA         ////////////////////////////
        output          VGA_HS,                 //  VGA H_SYNC
        output          VGA_VS,                 //  VGA V_SYNC
        output  [3:0]   VGA_R,                  //  VGA Red[3:0]
        output  [3:0]   VGA_G,                  //  VGA Green[3:0]
        output  [3:0]   VGA_B,                  //  VGA Blue[3:0]
        ////////////////////////    GPIO    ////////////////////////////////
        // input   [1:0]   GPIO0_CLKIN,            //  GPIO Connection 0 Clock In Bus
        // output  [1:0]   GPIO0_CLKOUT,           //  GPIO Connection 0 Clock Out Bus
        // inout   [31:0]  GPIO0_D,                //  GPIO Connection 0 Data Bus
        // input   [1:0]   GPIO1_CLKIN,            //  GPIO Connection 1 Clock In Bus
        // output  [1:0]   GPIO1_CLKOUT,           //  GPIO Connection 1 Clock Out Bus
        inout   [31:0]  GPIO1_D                 //  GPIO Connection 1 Data Bus
    );

// Generic useful debug counter.
reg [30:0] counter;

// What's displayed on the 7-segment display.
reg [15:0] debug_number;
reg [15:0] debug_number_nxt;
wire [15:0] debug_number_sdram_test;

// Button for resetting system, active low.
wire reset_n = BUTTON[2];

// The pixel clock of the video output, from the PLL.
wire vga_pixel_clock;

// Host-side VGA parameters.
wire    [3:0]   m_vga_r;
wire    [3:0]   m_vga_g;
wire    [3:0]   m_vga_b;
wire            m_vga_request;
wire            m_vga_top_of_screen;
reg             m_vga_top_of_screen_latched;

// VGA_side VGA parameters.
wire    [3:0]   s_vga_r;
wire    [3:0]   s_vga_g;
wire    [3:0]   s_vga_b;

// Hook up to pins to hardware.
assign  VGA_R   =   s_vga_r;
assign  VGA_G   =   s_vga_g;
assign  VGA_B   =   s_vga_b;

// Whether the SDRAM PLL has locked.
wire sdram_clk_locked;

// Debugging LEDs.
assign LEDG[0] = sdram_clk_locked;
assign LEDG[9:1] = counter[30:22];

// Clock that we're actually running our logic on.
wire our_clock = DRAM_CLK;

reg vga_fifo_clear;
reg vga_fifo_clear_nxt;
wire [11:0] vga_fifo_input;
wire vga_fifo_wrclk;
wire [11:0] vga_fifo_output;
wire vga_fifo_rdempty;
wire [7:0] vga_fifo_wrusedw;
wire vga_fifo_almost_full = vga_fifo_wrusedw[7];
reg vga_fifo_wrreq;
reg vga_fifo_wrreq_nxt;

// State machine states.
localparam STATE_START = 0;
localparam STATE_RESET = 1;
localparam STATE_GOING = 2;
localparam STATE_PAUSED = 3;
localparam STATE_STOPPED = 4;

reg [9:0] x;  // [0,640)
reg [9:0] x_nxt;
reg [9:0] y;  // [0,480)
reg [9:0] y_nxt;
reg [2:0] state;
reg [2:0] state_nxt;
reg [15:0] pixel_count;
reg [15:0] pixel_count_nxt;

// Convert the debug_number to four hex digits.
SEG7_LUT_4 seg7_lut_4(
    .oSEG0(HEX0_D),
    .oSEG0_DP(HEX0_DP),
    .oSEG1(HEX1_D),
    .oSEG1_DP(HEX1_DP),
    .oSEG2(HEX2_D),
    .oSEG2_DP(HEX2_DP),
    .oSEG3(HEX3_D),
    .oSEG3_DP(HEX3_DP),
    .iDIG(debug_number_sdram_test)
);

// Generate the VGA pixel clock.
Vga_clock vga_clock(
    .inclk0(CLOCK_50_2),
    .c0(vga_pixel_clock)
);

// Generate all signals for the VGA.
/*
Vga_control vga_control(
    // Host Side
    .oCurrent_X(),
    .oCurrent_Y(),
    .oAddress(),
    .iRed(m_vga_r),
    .iGreen(m_vga_g),
    .iBlue(m_vga_b),
    .oRequest(m_vga_request),
    .oTopOfScreen(m_vga_top_of_screen),
    // VGA Side
    .oVGA_R(s_vga_r),
    .oVGA_G(s_vga_g),
    .oVGA_B(s_vga_b),
    .oVGA_HS(VGA_HS),
    .oVGA_VS(VGA_VS),
    .oVGA_BLANK(),
    .oVGA_CLOCK(),
    // Control Signal
    .iCLK(vga_pixel_clock),
    .iRST_N(reset_n)
);
*/

// Generate all signals for the LCD.
LCD_control lcd_control(
    // Host Side
    .iRed(m_lcd_r),
    .iGreen(m_lcd_g),
    .iBlue(m_lcd_b),
    .oCurrent_X(lcd_x),
    .oCurrent_Y(lcd_y),
    .oAddress(),
    .oRequest(),
    .oTopOfScreen(),
    // LCD Side
    .oLCD_R(s_lcd_r),
    .oLCD_G(s_lcd_g),
    .oLCD_B(s_lcd_b),
    .oLCD_HS(lcd_hs),
    .oLCD_VS(lcd_vs),
    .oLCD_DE(lcd_de),
    // Control Signal
    .iCLK(vga_pixel_clock),
    .iRST_N(reset_n)
);

// FIFO to pass data from RAM to VGA.
VGA_FIFO vga_fifo(
    // Inputs.
    .aclr(vga_fifo_clear),
    .data(vga_fifo_input),
    .rdclk(vga_pixel_clock),
    .rdreq(m_vga_request),
    .wrclk(vga_fifo_wrclk),
    .wrreq(vga_fifo_wrreq),
    // Outputs.
    .q(vga_fifo_output),
    .rdempty(vga_fifo_rdempty),
    .wrfull(),
    .wrusedw(vga_fifo_wrusedw)
);

// PLL for the SDRAM clock.
SDRAM_clock sdram_clock(
   .areset(!reset_n),
   .inclk0(CLOCK_50),
   .c0(DRAM_CLK),
   .locked(sdram_clk_locked)
);

// Test of SDRAM.
// Convert from 13 to 12 bits for address.
wire [11:0] dram_addr;
assign DRAM_ADDR = { 1'b0, dram_addr };
SDRAM_test sdram_test(
    .reset_n(reset_n),
    .dram_dq(DRAM_DQ),
    .dram_addr(dram_addr),
    .dram_ldqm(DRAM_LDQM),
    .dram_udqm(DRAM_UDQM),
    .dram_we_n(DRAM_WE_N),
    .dram_cas_n(DRAM_CAS_N),
    .dram_ras_n(DRAM_RAS_N),
    .dram_cs_n(DRAM_CS_N),
    .dram_ba_0(DRAM_BA_0),
    .dram_ba_1(DRAM_BA_1),
    .dram_clk(DRAM_CLK),
    .dram_cke(DRAM_CKE),
    .dram_clk_locked(sdram_clk_locked),
    .debug_number(debug_number_sdram_test)
);

`ifdef NOTDEF
always @(posedge DRAM_CLK or negedge reset_n) begin
    if (!reset_n) begin
        counter <= 0;
    end
    else
    begin
        counter <= counter + 1'b1;
        /*
        if (counter == 50_000_000) begin
            counter <= 0;
        end
        */
    end
end
`endif

assign vga_fifo_input = (x == 0 || x == 639 || y == 0 || y == 479) ? 12'hF00 :
   (x[4] ^ y[4]) ? 12'h000 : 12'hFFF;
assign vga_fifo_wrclk = our_clock;

// Use magenta if the FIFO is empty.
assign m_vga_r = vga_fifo_rdempty ? 4'b1111 : vga_fifo_output[11:8];
assign m_vga_g = vga_fifo_rdempty ? 4'b0000 : vga_fifo_output[7:4];
assign m_vga_b = vga_fifo_rdempty ? 4'b1111 : vga_fifo_output[3:0];

// Initialize our variables.
initial begin
    counter <= 0;
    debug_number <= 16'h0000;
end

// Compute the next values of the input to the FIFO.
always @(*) begin
   state_nxt = state;
   x_nxt = x;
   y_nxt = y;
   vga_fifo_clear_nxt = vga_fifo_clear;
   vga_fifo_wrreq_nxt = 0;
   pixel_count_nxt = pixel_count;
   debug_number_nxt = debug_number;

   case (state)
      STATE_START: begin
         state_nxt = STATE_RESET;
         vga_fifo_clear_nxt = 1;
         // debug_number_nxt = debug_number + 1;
      end

      STATE_RESET: begin
         state_nxt = STATE_GOING;
         vga_fifo_clear_nxt = 0;
         x_nxt = 1'b0;
         y_nxt = 1'b0;
         pixel_count_nxt = 1;
      end

      STATE_GOING: begin
         if (vga_fifo_almost_full) begin
            state_nxt = STATE_PAUSED;
         end else begin
            if (x == 639) begin
               x_nxt = 1'b0;
               if (y == 479) begin
                  state_nxt = STATE_STOPPED;
                  // debug_number_nxt = pixel_count;
               end else begin
                  y_nxt = y + 1'b1;
                  vga_fifo_wrreq_nxt = 1'b1;
                  pixel_count_nxt = pixel_count + 1'b1;
               end
            end else begin
               x_nxt = x + 1'b1;
               vga_fifo_wrreq_nxt = 1'b1;
               pixel_count_nxt = pixel_count + 1'b1;
            end
         end
      end

      STATE_PAUSED: begin
         if (!vga_fifo_almost_full) begin
            state_nxt = STATE_GOING;
         end
      end

      STATE_STOPPED: begin
         // Nothing.
      end
   endcase
end

// Clock next values into the current ones.
always @(posedge our_clock) begin
    m_vga_top_of_screen_latched <= m_vga_top_of_screen;

    if (m_vga_top_of_screen_latched) begin
        state <= STATE_START;
        x <= 0;
        y <= 0;
        vga_fifo_clear <= 0;
        vga_fifo_wrreq <= 0;
    end else begin
        state <= state_nxt;
        x <= x_nxt;
        y <= y_nxt;
        vga_fifo_clear <= vga_fifo_clear_nxt;
        vga_fifo_wrreq <= vga_fifo_wrreq_nxt;
    end

    pixel_count <= pixel_count_nxt;
    debug_number <= debug_number_nxt;
end

// ****************** Nothing used below here.

`ifdef NOTDEF
reg signed [19:0] w0_row;
reg signed [19:0] w1_row;
reg signed [19:0] w2_row;
wire signed [19:0] w0;
wire signed [19:0] w1;
wire signed [19:0] w2;

assign w0 = 128_400 + m_vga_x*(-140) + m_vga_y*(-220);
assign w1 = -6000 + m_vga_x*(280) + m_vga_y*(-220);
assign w2 = -30000 + m_vga_x*(-140) + m_vga_y*(440);

wire insideTriangle = 1; // w0 >= 0 && w1 >= 0 && w2 >= 0;
//assign m_vga_r = insideTriangle ? (w0/6160) : 4'b0000;
//assign m_vga_g = insideTriangle ? (w1/6160) : 4'b0000;
//assign m_vga_b = insideTriangle ? (w2/6160) : 4'b0000;
`endif

`ifdef NOTDEF
always @(m_vga_x or m_vga_y)
  begin
    if (m_vga_x == 0 && m_vga_y == 0)
      begin
        w0_row <= 128_400;
        w1_row <= -6_000;
        w2_row <= -30_000;
        w0 <= w0_row;
        w1 <= w1_row;
        w2 <= w2_row;
      end
    else if (m_vga_x == 0)
      begin
        w0_row <= w0_row - 220;
        w1_row <= w1_row - 220;
        w2_row <= w2_row + 440;
        w0 <= w0_row;
        w1 <= w1_row;
        w2 <= w2_row;
      end
    else
      begin
        w0 <= w0 - 140;
        w1 <= w1 + 280;
        w2 <= w2 - 140;
      end
  end
`endif

// LCD test code.
reg [7:0] horiz_prescale;
reg [2:0] color_counter;
wire [7:0] red = color_counter[0] ? 8'hFF : 8'h00;
wire [7:0] green = color_counter[1] ? 8'hFF : 8'h00;
wire [7:0] blue = color_counter[2] ? 8'hFF : 8'h00;
wire [9:0] lcd_x;
wire [9:0] lcd_y;
wire lcd_checkerboard = lcd_x[4] ^ lcd_y[4];
wire [7:0] m_lcd_r = lcd_checkerboard ? red : 8'h00;
wire [7:0] m_lcd_g = lcd_checkerboard ? green : 8'h00;
wire [7:0] m_lcd_b = lcd_checkerboard ? blue : 8'h00;
wire [7:0] s_lcd_r;
wire [7:0] s_lcd_g;
wire [7:0] s_lcd_b;
wire lcd_hs;
wire lcd_vs;
wire lcd_de;
wire lcd_display_on = 1'b1;
assign { GPIO1_D[16:15], GPIO1_D[13:10] } = s_lcd_r[7:2];
assign { GPIO1_D[23:19], GPIO1_D[17] } = s_lcd_g[7:2];
assign { GPIO1_D[31], GPIO1_D[29:26], GPIO1_D[24] } = s_lcd_b[7:2];
assign GPIO1_D[2] = vga_pixel_clock;
assign GPIO1_D[4] = lcd_hs;
assign GPIO1_D[5] = lcd_vs;
assign GPIO1_D[7] = lcd_de;
assign GPIO1_D[8] = lcd_display_on;

always @(posedge vga_pixel_clock) begin
    if (!lcd_de) begin
        horiz_prescale <= 8'd0;
        color_counter <= 3'd1;
    end else begin
        if (horiz_prescale == 8'd115) begin
            horiz_prescale <= 8'd0;
            color_counter <= color_counter + 3'd1;
        end else begin
            horiz_prescale <= horiz_prescale + 8'd1;
        end
    end
end

endmodule
