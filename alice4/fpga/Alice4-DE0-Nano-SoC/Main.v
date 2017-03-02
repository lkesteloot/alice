
//`define ENABLE_ADC
//`define ENABLE_ARDUINO
`define ENABLE_GPIO0
//`define ENABLE_GPIO1
//`define ENABLE_HPS

module Main(
        //////////// CLOCK //////////
        input clock_50,
        input clock2_50,
        input clock3_50,

`ifdef ENABLE_ADC
        //////////// ADC //////////
        /* 3.3-V LVTTL */
        output adc_convst,
        output adc_sclk,
        output adc_sdi,
        input adc_sdo,
`endif
        
`ifdef ENABLE_ARDUINO
        //////////// ARDUINO ////////////
        /* 3.3-V LVTTL */
        inout [15:0] arduino_io,
        inout arduino_reset_n,
`endif
        
`ifdef ENABLE_GPIO0
        //////////// GPIO 0 ////////////
        /* 3.3-V LVTTL */
        inout [35:0] gpio_0,
`endif

`ifdef ENABLE_GPIO1     
        //////////// GPIO 1 ////////////
        /* 3.3-V LVTTL */
        inout [35:0] gpio_1,
`endif

`ifdef ENABLE_HPS
        //////////// HPS //////////
        /* 3.3-V LVTTL */
        inout hps_conv_usb_n,
        
        /* SSTL-15 Class I */
        output [14:0] hps_ddr3_addr,
        output [2:0] hps_ddr3_ba,
        output hps_ddr3_cas_n,
        output hps_ddr3_cke,
        output hps_ddr3_cs_n,
        output [3:0] hps_ddr3_dm,
        inout [31:0] hps_ddr3_dq,
        output hps_ddr3_odt,
        output hps_ddr3_ras_n,
        output hps_ddr3_reset_n,
        input hps_ddr3_rzq,
        output hps_ddr3_we_n,
        /* DIFFERENTIAL 1.5-V SSTL CLASS I */
        output hps_ddr3_ck_n,
        output hps_ddr3_ck_p,
        inout [3:0] hps_ddr3_dqs_n,
        inout [3:0] hps_ddr3_dqs_p,
        
        /* 3.3-V LVTTL */
        output hps_enet_gtx_clk,
        inout hps_enet_int_n,
        output hps_enet_mdc,
        inout hps_enet_mdio,
        input hps_enet_rx_clk,
        input [3:0] hps_enet_rx_data,
        input hps_enet_rx_dv,
        output [3:0] hps_enet_tx_data,
        output hps_enet_tx_en,
        inout hps_gsensor_int,
        inout hps_i2c0_sclk,
        inout hps_i2c0_sdat,
        inout hps_i2c1_sclk,
        inout hps_i2c1_sdat,
        inout hps_key,
        inout hps_led,
        inout hps_ltc_gpio,
        output hps_sd_clk,
        inout hps_sd_cmd,
        inout [3:0] hps_sd_data,
        output hps_spim_clk,
        input hps_spim_miso,
        output hps_spim_mosi,
        inout hps_spim_ss,
        input hps_uart_rx,
        output hps_uart_tx,
        input hps_usb_clkout,
        inout [7:0] hps_usb_data,
        input hps_usb_dir,
        input hps_usb_nxt,
        output hps_usb_stp,
`endif
        
        //////////// KEY ////////////
        /* 3.3-V LVTTL */
        input [1:0] key,
        
        //////////// LED ////////////
        /* 3.3-V LVTTL */
        output [7:0] led,
        
        //////////// SW ////////////
        /* 3.3-V LVTTL */
        input [3:0] sw
);

    // Debug LED blink.
    reg [23:0] counter;
    always @(posedge clock_50) begin
       counter <= counter + 1'b1;
    end
    assign led[0] = counter[23];

    // Reset.
    wire reset_n = key[0];

    // Generate signals for the LCD.
    wire [9:0] lcd_x;
    wire [9:0] lcd_y;
    reg lcd_clock;
    wire lcd_data_enable;
    wire lcd_hs_n;
    wire lcd_vs_n;
    wire lcd_display_on = sw[0];
    LCD_control lcd_control(
        .clock(clock_50),
        .tick(lcd_clock),
        .reset_n(reset_n),
        .x(lcd_x),
        .y(lcd_y),
        .next_frame(),
        .hs_n(lcd_hs_n),
        .vs_n(lcd_vs_n),
        .data_enable(lcd_data_enable)
    );
    
    // Generate character locations.
    wire [6:0] text_column;
    wire [5:0] text_row;
    wire [2:0] character_column;
    wire [3:0] character_row;
    LCD_text #(.LOG2_SCALE(2)) lcd_text(
        .x(lcd_x),
        .y(lcd_y),
        .column(text_column),
        .row(text_row),
        .character_column(character_column),
        .character_row(character_row)
    );
    
    // Generate characters themselves.
    wire [6:0] character;
    LCD_test lcd_test(
        .clock(clock_50),
        .reset_n(reset_n),
        .column(text_column),
        .row(text_row),
        .character(character)
    );
    
    // Generate pixels.
    wire character_bw;
    LCD_font lcd_font(
        .clock(clock_50),
        .character(character),
        .character_column(character_column),
        .character_row(character_row),
        .bw(character_bw)
    );
    
    // Color assignment. Latch these for clean output.
    reg [7:0] lcd_red;
    reg [7:0] lcd_green;
    reg [7:0] lcd_blue;
    wire [7:0] gray = character_bw ? 8'hFF : 8'h00;
    wire [7:0] lcd_red_next = gray;
    wire [7:0] lcd_green_next = gray;
    wire [7:0] lcd_blue_next = gray;
    always @(posedge clock_50) begin
        if (lcd_clock) begin
            lcd_red <= lcd_red_next;
            lcd_green <= lcd_green_next;
            lcd_blue <= lcd_blue_next;
        end
    end

    // GPIO pins.
    assign gpio_0[4] = lcd_clock;
    assign gpio_0[6] = lcd_hs_n;
    assign gpio_0[7] = lcd_vs_n;
    assign gpio_0[9] = lcd_data_enable;
    assign gpio_0[10] = lcd_display_on;
    assign gpio_0[15:12] = lcd_red[5:2];
    assign gpio_0[20:19] = lcd_red[7:6];
    assign gpio_0[21] = lcd_green[2];
    assign gpio_0[27:23] = lcd_green[7:3];
    assign gpio_0[28] = lcd_blue[2];
    assign gpio_0[33:30] = lcd_blue[6:3];
    assign gpio_0[35] = lcd_blue[7];

    // LCD clock.
    always @(posedge clock_50) begin
        // 25 MHz.
        lcd_clock <= ~lcd_clock;
    end

endmodule
