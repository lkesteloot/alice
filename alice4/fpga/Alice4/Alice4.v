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
        ////////////////////    Clock Input     ////////////////////     
        CLOCK_50,                       //  50 MHz
        CLOCK_50_2,                     //  50 MHz
        ////////////////////    Push Button     ////////////////////
        BUTTON,                         //  Pushbutton[2:0]
        ////////////////////    DPDT Switch     ////////////////////
        SW,                             //  Toggle Switch[9:0]
        ////////////////////    7-SEG Dispaly   ////////////////////
        HEX0_D,                         //  Seven Segment Digit 0
        HEX0_DP,                        //  Seven Segment Digit DP 0
        HEX1_D,                         //  Seven Segment Digit 1
        HEX1_DP,                        //  Seven Segment Digit DP 1
        HEX2_D,                         //  Seven Segment Digit 2
        HEX2_DP,                        //  Seven Segment Digit DP 2
        HEX3_D,                         //  Seven Segment Digit 3
        HEX3_DP,                        //  Seven Segment Digit DP 3
        ////////////////////////    LED     ////////////////////////
        LEDG,                           //  LED Green[9:0]
        ////////////////////////    UART    ////////////////////////
        UART_TXD,                       //  UART Transmitter
        UART_RXD,                       //  UART Receiver
        UART_CTS,                       //  UART Clear To Send
        UART_RTS,                       //  UART Request To Send
        /////////////////////   SDRAM Interface     ////////////////
        DRAM_DQ,                        //  SDRAM Data bus 16 Bits
        DRAM_ADDR,                      //  SDRAM Address bus 13 Bits
        DRAM_LDQM,                      //  SDRAM Low-byte Data Mask 
        DRAM_UDQM,                      //  SDRAM High-byte Data Mask
        DRAM_WE_N,                      //  SDRAM Write Enable
        DRAM_CAS_N,                     //  SDRAM Column Address Strobe
        DRAM_RAS_N,                     //  SDRAM Row Address Strobe
        DRAM_CS_N,                      //  SDRAM Chip Select
        DRAM_BA_0,                      //  SDRAM Bank Address 0
        DRAM_BA_1,                      //  SDRAM Bank Address 1
        DRAM_CLK,                       //  SDRAM Clock
        DRAM_CKE,                       //  SDRAM Clock Enable
        ////////////////////    Flash Interface     ////////////////
        FL_DQ,                          //  FLASH Data bus 15 Bits
        FL_DQ15_AM1,                    //  FLASH Data bus Bit 15 or Address A-1
        FL_ADDR,                        //  FLASH Address bus 22 Bits
        FL_WE_N,                        //  FLASH Write Enable
        FL_RST_N,                       //  FLASH Reset
        FL_OE_N,                        //  FLASH Output Enable
        FL_CE_N,                        //  FLASH Chip Enable
        FL_WP_N,                        //  FLASH Hardware Write Protect
        FL_BYTE_N,                      //  FLASH Selects 8/16-bit mode
        FL_RY,                          //  FLASH Ready/Busy
        ////////////////////    LCD Module 16X2     ////////////////
        LCD_BLON,                       //  LCD Back Light ON/OFF
        LCD_RW,                         //  LCD Read/Write Select, 0 = Write, 1 = Read
        LCD_EN,                         //  LCD Enable
        LCD_RS,                         //  LCD Command/Data Select, 0 = Command, 1 = Data
        LCD_DATA,                       //  LCD Data bus 8 bits
        ////////////////////    SD_Card Interface   ////////////////
        SD_DAT0,                        //  SD Card Data 0
        SD_DAT3,                        //  SD Card Data 3
        SD_CMD,                         //  SD Card Command Signal
        SD_CLK,                         //  SD Card Clock
        SD_WP_N,                        //  SD Card Write Protect
        ////////////////////    PS2     ////////////////////////////
        PS2_KBDAT,                      //  PS2 Keyboard Data
        PS2_KBCLK,                      //  PS2 Keyboard Clock
        PS2_MSDAT,                      //  PS2 Mouse Data
        PS2_MSCLK,                      //  PS2 Mouse Clock
        ////////////////////    VGA     ////////////////////////////
        VGA_HS,                         //  VGA H_SYNC
        VGA_VS,                         //  VGA V_SYNC
        VGA_R,                          //  VGA Red[3:0]
        VGA_G,                          //  VGA Green[3:0]
        VGA_B,                          //  VGA Blue[3:0]
        ////////////////////    GPIO    ////////////////////////////
        GPIO0_CLKIN,                    //  GPIO Connection 0 Clock In Bus
        GPIO0_CLKOUT,                   //  GPIO Connection 0 Clock Out Bus
        GPIO0_D,                        //  GPIO Connection 0 Data Bus
        GPIO1_CLKIN,                    //  GPIO Connection 1 Clock In Bus
        GPIO1_CLKOUT,                   //  GPIO Connection 1 Clock Out Bus
        GPIO1_D                         //  GPIO Connection 1 Data Bus
    );

////////////////////////    Clock Input     ////////////////////////
input           CLOCK_50;               //  50 MHz
input           CLOCK_50_2;             //  50 MHz
////////////////////////    Push Button     ////////////////////////
input   [2:0]   BUTTON;                 //  Pushbutton[2:0]
////////////////////////    DPDT Switch     ////////////////////////
input   [9:0]   SW;                     //  Toggle Switch[9:0]
////////////////////////    7-SEG Dispaly   ////////////////////////
output  [6:0]   HEX0_D;                 //  Seven Segment Digit 0
output          HEX0_DP;                //  Seven Segment Digit DP 0
output  [6:0]   HEX1_D;                 //  Seven Segment Digit 1
output          HEX1_DP;                //  Seven Segment Digit DP 1
output  [6:0]   HEX2_D;                 //  Seven Segment Digit 2
output          HEX2_DP;                //  Seven Segment Digit DP 2
output  [6:0]   HEX3_D;                 //  Seven Segment Digit 3
output          HEX3_DP;                //  Seven Segment Digit DP 3
////////////////////////////    LED     ////////////////////////////
output  [9:0]   LEDG;                   //  LED Green[9:0]
////////////////////////////    UART    ////////////////////////////
output          UART_TXD;               //  UART Transmitter
input           UART_RXD;               //  UART Receiver
output          UART_CTS;               //  UART Clear To Send
input           UART_RTS;               //  UART Request To Send
///////////////////////     SDRAM Interface ////////////////////////
inout   [15:0]  DRAM_DQ;                //  SDRAM Data bus 16 Bits
output  [12:0]  DRAM_ADDR;              //  SDRAM Address bus 13 Bits
output          DRAM_LDQM;              //  SDRAM Low-byte Data Mask
output          DRAM_UDQM;              //  SDRAM High-byte Data Mask
output          DRAM_WE_N;              //  SDRAM Write Enable
output          DRAM_CAS_N;             //  SDRAM Column Address Strobe
output          DRAM_RAS_N;             //  SDRAM Row Address Strobe
output          DRAM_CS_N;              //  SDRAM Chip Select
output          DRAM_BA_0;              //  SDRAM Bank Address 0
output          DRAM_BA_1;              //  SDRAM Bank Address 1
output          DRAM_CLK;               //  SDRAM Clock
output          DRAM_CKE;               //  SDRAM Clock Enable
////////////////////////    Flash Interface ////////////////////////
inout   [14:0]  FL_DQ;                  //  FLASH Data bus 15 Bits
inout           FL_DQ15_AM1;            //  FLASH Data bus Bit 15 or Address A-1
output  [21:0]  FL_ADDR;                //  FLASH Address bus 22 Bits
output          FL_WE_N;                //  FLASH Write Enable
output          FL_RST_N;               //  FLASH Reset
output          FL_OE_N;                //  FLASH Output Enable
output          FL_CE_N;                //  FLASH Chip Enable
output          FL_WP_N;                //  FLASH Hardware Write Protect
output          FL_BYTE_N;              //  FLASH Selects 8/16-bit mode
input           FL_RY;                  //  FLASH Ready/Busy
////////////////////    LCD Module 16X2 ////////////////////////////
inout   [7:0]   LCD_DATA;               //  LCD Data bus 8 bits
output          LCD_BLON;               //  LCD Back Light ON/OFF
output          LCD_RW;                 //  LCD Read/Write Select, 0 = Write, 1 = Read
output          LCD_EN;                 //  LCD Enable
output          LCD_RS;                 //  LCD Command/Data Select, 0 = Command, 1 = Data
////////////////////    SD Card Interface   ////////////////////////
inout           SD_DAT0;                //  SD Card Data 0
inout           SD_DAT3;                //  SD Card Data 3
inout           SD_CMD;                 //  SD Card Command Signal
output          SD_CLK;                 //  SD Card Clock
input           SD_WP_N;                //  SD Card Write Protect
////////////////////////    PS2     ////////////////////////////////
inout           PS2_KBDAT;              //  PS2 Keyboard Data
inout           PS2_KBCLK;              //  PS2 Keyboard Clock
inout           PS2_MSDAT;              //  PS2 Mouse Data
inout           PS2_MSCLK;              //  PS2 Mouse Clock
////////////////////////    VGA         ////////////////////////////
output          VGA_HS;                 //  VGA H_SYNC
output          VGA_VS;                 //  VGA V_SYNC
output  [3:0]   VGA_R;                  //  VGA Red[3:0]
output  [3:0]   VGA_G;                  //  VGA Green[3:0]
output  [3:0]   VGA_B;                  //  VGA Blue[3:0]
////////////////////////    GPIO    ////////////////////////////////
input   [1:0]   GPIO0_CLKIN;            //  GPIO Connection 0 Clock In Bus
output  [1:0]   GPIO0_CLKOUT;           //  GPIO Connection 0 Clock Out Bus
inout   [31:0]  GPIO0_D;                //  GPIO Connection 0 Data Bus
input   [1:0]   GPIO1_CLKIN;            //  GPIO Connection 1 Clock In Bus
output  [1:0]   GPIO1_CLKOUT;           //  GPIO Connection 1 Clock Out Bus
inout   [31:0]  GPIO1_D;                //  GPIO Connection 1 Data Bus

//=======================================================
//  REG/WIRE declarations
//=======================================================

reg [25:0] counter;
reg [15:0] seconds;
reg [15:0] debug_number;     // What's displayed on the 7-segment display.
reg [15:0] debug_number_nxt;
assign LEDG[9:0] = seconds[9:0];
wire reset_n = BUTTON[2];
wire Vga_pixel_clock;
wire    [9:0]   mVGA_X;
wire    [9:0]   mVGA_Y;
wire    [3:0]   mVGA_R;
wire    [3:0]   mVGA_G;
wire    [3:0]   mVGA_B;
wire            mVGA_request;
wire            mVGA_top_of_screen;
//wire  [19:0]  mVGA_ADDR;
wire    [3:0]   sVGA_R;
wire    [3:0]   sVGA_G;
wire    [3:0]   sVGA_B;
// Hook up to pins (LSB only).
assign  VGA_R   =   sVGA_R;
assign  VGA_G   =   sVGA_G;
assign  VGA_B   =   sVGA_B;

//=======================================================
//  Structural coding
//=======================================================

always @(posedge CLOCK_50 or negedge reset_n) begin
    if (!reset_n) begin
        counter <= 0;
        seconds <= 0;
    end
    else
    begin
        counter <= counter + 1'b1;
        if (counter == 50_000_000) begin
            counter <= 0;
            seconds <= seconds + 1'b1;
        end
    end
end

SEG7_LUT_4 u1(
    .oSEG0(HEX0_D),
    .oSEG0_DP(HEX0_DP),
    .oSEG1(HEX1_D),
    .oSEG1_DP(HEX1_DP),
    .oSEG2(HEX2_D),
    .oSEG2_DP(HEX2_DP),
    .oSEG3(HEX3_D),
    .oSEG3_DP(HEX3_DP),
    .iDIG(debug_number)
);

Vga_clock u2(
    .inclk0(CLOCK_50),
    .c0(Vga_pixel_clock)
);

Vga_control u3 (
    // Host Side
    .oCurrent_X(mVGA_X),
    .oCurrent_Y(mVGA_Y),
    .iRed(mVGA_R),
    .iGreen(mVGA_G),
    .iBlue(mVGA_B),
	 .oRequest(mVGA_request),
	 .oTopOfScreen(mVGA_top_of_screen),
    // VGA Side
    .oVGA_R(sVGA_R),
    .oVGA_G(sVGA_G),
    .oVGA_B(sVGA_B),
    .oVGA_HS(VGA_HS),
    .oVGA_VS(VGA_VS),
    .oVGA_BLANK(),
    .oVGA_CLOCK(),
    // Control Signal
    .iCLK(Vga_pixel_clock),
    .iRST_N(reset_n)
);

// FIFO to pass data from RAM to VGA.
VGA_FIFO u4 (
    // Inputs.
	 .aclr(Vga_fifo_clear),
	 .data(Vga_fifo_input),
	 .rdclk(Vga_pixel_clock),
	 .rdreq(mVGA_request),
	 .wrclk(Vga_fifo_wrclk),
	 .wrreq(Vga_fifo_wrreq),
	 // Outputs.
	 .q(Vga_fifo_output),
	 .rdempty(Vga_fifo_rdempty),
	 .wrfull(Vga_fifo_wrfull),
	 .wrusedw(Vga_fifo_wrusedw)
);

reg Vga_fifo_clear;
reg Vga_fifo_clear_nxt;
wire [15:0] Vga_fifo_input;
wire Vga_fifo_wrclk;
wire [15:0] Vga_fifo_output;
wire Vga_fifo_rdempty;
wire Vga_fifo_wrfull;
wire [7:0] Vga_fifo_wrusedw;
wire Vga_fifo_almost_full = Vga_fifo_wrusedw[7];

assign Vga_fifo_input = (x == 0 || x == 639 || y == 0 || y == 479) ? 12'hF00 :
	(x[4] ^ y[4]) ? 12'h000 : 12'hFFF;
assign Vga_fifo_wrclk = CLOCK_50;
reg Vga_fifo_wrreq;
reg Vga_fifo_wrreq_nxt;

// Use magenta if the FIFO is empty.
assign mVGA_R = Vga_fifo_rdempty ? 4'b1111 : Vga_fifo_output[11:8];
assign mVGA_G = Vga_fifo_rdempty ? 4'b0000 : Vga_fifo_output[7:4];
assign mVGA_B = Vga_fifo_rdempty ? 4'b1111 : Vga_fifo_output[3:0];

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

always @(*) begin
   state_nxt = state;
	x_nxt = x;
	y_nxt = y;
	Vga_fifo_clear_nxt = Vga_fifo_clear;
	Vga_fifo_wrreq_nxt = 0;
	pixel_count_nxt = pixel_count;
	debug_number_nxt = debug_number;

	case (state)
	   STATE_START: begin
		   state_nxt = STATE_RESET;
			Vga_fifo_clear_nxt = 1;
		end
		
		STATE_RESET: begin
			state_nxt = STATE_GOING;
			Vga_fifo_clear_nxt = 0;
			x_nxt = 1'b0;
			y_nxt = 1'b0;
			pixel_count_nxt = 1;
		end
		
		STATE_GOING: begin
			if (Vga_fifo_almost_full) begin
				state_nxt = STATE_PAUSED;
			end else begin
				if (x == 639) begin
					x_nxt = 1'b0;
					if (y == 479) begin
						state_nxt = STATE_STOPPED;
						debug_number_nxt = pixel_count;
					end else begin
						y_nxt = y + 1'b1;
						Vga_fifo_wrreq_nxt = 1'b1;
						pixel_count_nxt = pixel_count + 1'b1;
					end
				end else begin
					x_nxt = x + 1'b1;
					Vga_fifo_wrreq_nxt = 1'b1;
					pixel_count_nxt = pixel_count + 1'b1;
				end
			end
		end
		
		STATE_PAUSED: begin
			if (!Vga_fifo_almost_full) begin
				state_nxt = STATE_GOING;
			end
		end
		
		STATE_STOPPED: begin
		   // Nothing.
	   end
	endcase
end

always @(posedge CLOCK_50) begin
	if (mVGA_top_of_screen) begin
	   state <= STATE_START;
		x <= 0;
		y <= 0;
		Vga_fifo_clear <= 0;
		Vga_fifo_wrreq <= 0;
	end else begin
      state <= state_nxt;
      x <= x_nxt;
      y <= y_nxt;
		Vga_fifo_clear <= Vga_fifo_clear_nxt;
		Vga_fifo_wrreq <= Vga_fifo_wrreq_nxt;
	end

	pixel_count <= pixel_count_nxt;
	debug_number <= debug_number_nxt;
end

`ifdef NOTDEF
reg [17:0] tmpcnt;
always @(posedge CLOCK_50 or posedge mVGA_top_of_screen) begin
   if (mVGA_top_of_screen) begin
	   tmpcnt <= 18'd153_600;
	end
	else
	begin
      if (tmpcnt != 0 && !Vga_fifo_wrfull) begin
	   	tmpcnt <= tmpcnt - 18'b1;
	   end
	end
end
`endif


`ifdef NOTDEF
reg signed [19:0] w0_row;
reg signed [19:0] w1_row;
reg signed [19:0] w2_row;
wire signed [19:0] w0;
wire signed [19:0] w1;
wire signed [19:0] w2;

assign w0 = 128_400 + mVGA_X*(-140) + mVGA_Y*(-220);
assign w1 = -6000 + mVGA_X*(280) + mVGA_Y*(-220);
assign w2 = -30000 + mVGA_X*(-140) + mVGA_Y*(440);

wire insideTriangle = 1; // w0 >= 0 && w1 >= 0 && w2 >= 0;
//assign mVGA_R = insideTriangle ? (w0/6160) : 4'b0000;
//assign mVGA_G = insideTriangle ? (w1/6160) : 4'b0000;
//assign mVGA_B = insideTriangle ? (w2/6160) : 4'b0000;
`endif

`ifdef NOTDEF
always @(mVGA_X or mVGA_Y)
  begin
    if (mVGA_X == 0 && mVGA_Y == 0)
	   begin
		  w0_row <= 128_400;
		  w1_row <= -6_000;
		  w2_row <= -30_000;
		  w0 <= w0_row;
		  w1 <= w1_row;
		  w2 <= w2_row;
		end
	 else if (mVGA_X == 0)
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

endmodule
