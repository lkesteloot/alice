
//`define ENABLE_ADC
//`define ENABLE_ARDUINO
`define ENABLE_GPIO0
//`define ENABLE_GPIO1

module Main(
        //////////// CLOCK //////////
        input clock_50,
        /// input clock2_50,   // Not assigned in SDC file.
        /// input clock3_50,   // Not assigned in SDC file.

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

        //////////// HPS //////////
        /* 3.3-V LVTTL */
        /// inout hps_conv_usb_n,
        
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
        /*
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
        */

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

    // 1G minus 128M, in bytes.
    localparam FRAME_BUFFER_ADDRESS = 30'h3800_0000;
    // Size of frame buffer in pixels.
    localparam FRAME_BUFFER_WIDTH = 800;
    localparam FRAME_BUFFER_HEIGHT = 480;
    // Number of bytes in frame buffer.
    localparam FRAME_BUFFER_LENGTH = FRAME_BUFFER_WIDTH*FRAME_BUFFER_HEIGHT*4;
    // Position of protocol buffer.
    localparam CMD_ADDRESS = FRAME_BUFFER_ADDRESS + 3*FRAME_BUFFER_LENGTH;

    // Debug LED blink.
    reg [23:0] counter;
    always @(posedge clock_50) begin
       counter <= counter + 1'b1;
    end
    assign led[0] = counter[23];

    // Reset.
    wire reset_n = key[0];
    assign led[1] = !reset_n;

    // Exchange data with HPS.
    wire [31:0] f2h_value = {
        29'b0,
        rast_front_buffer,
        fb_front_buffer,
        rasterizer_busy
    };
    wire [31:0] h2f_value;
    cyclonev_hps_interface_mpu_general_purpose h2f_gp(
         .gp_in(f2h_value),    // Value to the HPS (continuous).
         .gp_out(h2f_value)    // Value from the HPS (latched).
    );

    // Interface to HPS.
    wire [28:0] sdram0_address;
    wire [7:0] sdram0_burstcount;
    wire sdram0_waitrequest;
    wire [63:0] sdram0_readdata;
    wire sdram0_readdatavalid;
    wire sdram0_read;
    wire [28:0] sdram1_address;
    wire [7:0] sdram1_burstcount;
    wire sdram1_waitrequest;
    wire [63:0] sdram1_readdata;
    wire sdram1_readdatavalid;
    wire sdram1_read;
    wire [63:0] sdram1_writedata;
    wire [7:0] sdram1_byteenable;
    wire sdram1_write;
    soc_system soc_system_instance(
        .clk_clk(clock_50),
        .memory_mem_a(hps_ddr3_addr),
        .memory_mem_ba(hps_ddr3_ba),
        .memory_mem_ck(hps_ddr3_ck_p),
        .memory_mem_ck_n(hps_ddr3_ck_n),
        .memory_mem_cke(hps_ddr3_cke),
        .memory_mem_cs_n(hps_ddr3_cs_n),
        .memory_mem_ras_n(hps_ddr3_ras_n),
        .memory_mem_cas_n(hps_ddr3_cas_n),
        .memory_mem_we_n(hps_ddr3_we_n),
        .memory_mem_reset_n(hps_ddr3_reset_n),
        .memory_mem_dq(hps_ddr3_dq),
        .memory_mem_dqs(hps_ddr3_dqs_p),
        .memory_mem_dqs_n(hps_ddr3_dqs_n),
        .memory_mem_odt(hps_ddr3_odt),
        .memory_mem_dm(hps_ddr3_dm),
        .memory_oct_rzqin(hps_ddr3_rzq),
        .hps_0_f2h_sdram0_data_address(sdram0_address),
        .hps_0_f2h_sdram0_data_burstcount(sdram0_burstcount),
        .hps_0_f2h_sdram0_data_waitrequest(sdram0_waitrequest),
        .hps_0_f2h_sdram0_data_readdata(sdram0_readdata),
        .hps_0_f2h_sdram0_data_readdatavalid(sdram0_readdatavalid),
        .hps_0_f2h_sdram0_data_read(sdram0_read),
        .hps_0_f2h_sdram1_data_address(sdram1_address),
        .hps_0_f2h_sdram1_data_burstcount(sdram1_burstcount),
        .hps_0_f2h_sdram1_data_waitrequest(sdram1_waitrequest),
        .hps_0_f2h_sdram1_data_readdata(sdram1_readdata),
        .hps_0_f2h_sdram1_data_readdatavalid(sdram1_readdatavalid),
        .hps_0_f2h_sdram1_data_read(sdram1_read),
        .hps_0_f2h_sdram1_data_writedata(sdram1_writedata),
        .hps_0_f2h_sdram1_data_byteenable(sdram1_byteenable),
        .hps_0_f2h_sdram1_data_write(sdram1_write)
    );

    // Generate signals for the LCD.
    wire [9:0] lcd_x;
    wire [9:0] lcd_y;
    // This is both a tick and a clock, which we can do because it's
    // half the speed of clock_50.
    reg lcd_tick;
    wire lcd_data_enable;
    wire lcd_hs_n;
    wire lcd_vs_n;
    wire lcd_display_on = 1'b1;
    wire lcd_pwm;
    wire next_frame;
    LCD_control lcd_control(
        .clock(clock_50),
        .tick(lcd_tick),
        .reset_n(reset_n),
        .x(lcd_x),
        .y(lcd_y),
        .next_frame(next_frame),
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
    LCD_debug lcd_debug(
        .column(text_column),
        .row(text_row),
        .value0(rast_debug_value0),
        .value1(rast_debug_value1),
        .value2(rast_debug_value2),
        .character(character)
    );

    // Frame buffer.
    wire [31:0] fb_debug_value0;
    wire [31:0] fb_debug_value1;
    wire [31:0] fb_debug_value2;
    wire rast_front_buffer;
    wire fb_front_buffer;
    Frame_buffer #(.ADDRESS(FRAME_BUFFER_ADDRESS),
                   .LENGTH(FRAME_BUFFER_LENGTH)) frame_buffer(
        .clock(clock_50),
        .reset_n(reset_n),

        // Memory interface:
        .address(sdram0_address),
        .burstcount(sdram0_burstcount),
        .waitrequest(sdram0_waitrequest),
        .readdata(sdram0_readdata),
        .readdatavalid(sdram0_readdatavalid),
        .read(sdram0_read),

        // Display interface:
        .lcd_tick(lcd_tick),
        .lcd_next_frame(next_frame),
        .lcd_red(fb_red),
        .lcd_green(fb_green),
        .lcd_blue(fb_blue),
        .lcd_data_enable(lcd_data_enable),

        // Front buffer handling.
        .rast_front_buffer(rast_front_buffer),
        .fb_front_buffer(fb_front_buffer),

        // Debugging:
        .debug_value0(fb_debug_value0),
        .debug_value1(fb_debug_value1),
        .debug_value2(fb_debug_value2)
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

    // Rasterizer.
    wire rasterizer_data_ready = h2f_value[0];
    wire rasterizer_busy;
    wire [31:0] rast_debug_value0;
    wire [31:0] rast_debug_value1;
    wire [31:0] rast_debug_value2;
    Rasterizer #(.FB_ADDRESS(FRAME_BUFFER_ADDRESS),
                 .FB_LENGTH(FRAME_BUFFER_LENGTH),
                 .FB_WIDTH(FRAME_BUFFER_WIDTH),
                 .CMD_ADDRESS(CMD_ADDRESS)) rasterizer(

        .clock(clock_50),
        .reset_n(reset_n),
        .test_mode(sw[0]),
        .data_ready(rasterizer_data_ready),
        .busy(rasterizer_busy),

        // Memory:
        .address(sdram1_address),
        .burstcount(sdram1_burstcount),
        .waitrequest(sdram1_waitrequest),
        .readdata(sdram1_readdata),
        .readdatavalid(sdram1_readdatavalid),
        .read(sdram1_read),
        .writedata(sdram1_writedata),
        .byteenable(sdram1_byteenable),
        .write(sdram1_write),

        // Front buffer handling.
        .rast_front_buffer(rast_front_buffer),
        .fb_front_buffer(fb_front_buffer),

        // Debugging:
        .debug_value0(rast_debug_value0),
        .debug_value1(rast_debug_value1),
        .debug_value2(rast_debug_value2)
    );

    // Color assignment. Latch these for clean output.
    wire [7:0] fb_red;
    wire [7:0] fb_green;
    wire [7:0] fb_blue;
    wire [7:0] lcd_red = character_bw_latched && sw[3] ? 8'h80 : fb_red;
    wire [7:0] lcd_green = character_bw_latched && sw[3] ? 8'h00 : fb_green;
    wire [7:0] lcd_blue = character_bw_latched && sw[3] ? 8'h00 : fb_blue;
    reg lcd_data_enable_delayed;
    reg character_bw_latched;
    always @(posedge clock_50) begin
        if (lcd_tick) begin
            // We must delay lcd_data_enable by one clock because
            // the frame buffer has sent us delayed color.
            lcd_data_enable_delayed <= lcd_data_enable;
            character_bw_latched <= character_bw;
        end
    end

    // GPIO pins.
    assign gpio_0[0] = lcd_red[0];
    assign gpio_0[2] = lcd_red[1];
    assign gpio_0[4] = lcd_red[2];
    assign gpio_0[6] = lcd_red[3];
    assign gpio_0[8] = lcd_red[4];
    assign gpio_0[10] = lcd_red[5];
    assign gpio_0[12] = lcd_red[6];
    assign gpio_0[14] = lcd_red[7];

    assign gpio_0[16] = lcd_blue[0];
    assign gpio_0[18] = lcd_blue[1];
    assign gpio_0[20] = lcd_blue[2];
    assign gpio_0[22] = lcd_blue[3];
    assign gpio_0[24] = lcd_blue[4];
    assign gpio_0[26] = lcd_blue[5];
    assign gpio_0[28] = lcd_blue[6];
    assign gpio_0[30] = lcd_blue[7];

    assign gpio_0[1] = lcd_green[0];
    assign gpio_0[3] = lcd_green[1];
    assign gpio_0[5] = lcd_green[2];
    assign gpio_0[7] = lcd_green[3];
    assign gpio_0[9] = lcd_green[4];
    assign gpio_0[11] = lcd_green[5];
    assign gpio_0[13] = lcd_green[6];
    assign gpio_0[15] = lcd_green[7];

    assign gpio_0[17] = lcd_tick;
    assign gpio_0[19] = lcd_display_on;
    assign gpio_0[21] = sw[1] ? 1'b1 : lcd_hs_n;
    assign gpio_0[23] = sw[2] ? 1'b1 : lcd_vs_n;
    assign gpio_0[25] = lcd_data_enable_delayed;
    assign gpio_0[27] = lcd_pwm;

    // LCD clock.
    always @(posedge clock_50) begin
        // 25 MHz.
        lcd_tick <= ~lcd_tick;
    end

    // LCD backlight PWM.
    // Brightness value between 0 and 1000 inclusive, where both
    // ends cause no waveform at all.
    localparam PWM_MAX_VALUE = 1000;
    localparam PWM_BITS = 10;
    wire pwm_tick;
    // Get the brightness from the CPU, unless bit 1 is 0, in which
    // case make it full brightness. This is to handle the power-on case.
    wire [PWM_BITS-1:0] pwm_value =
        h2f_value[1] ? h2f_value[PWM_BITS-1+2:2] : PWM_MAX_VALUE;
    // PWM frequency of 200 Hz (50 MHz / 250 / PWM_MAX_VALUE). We tried
    // 1 kHz and 5 kHz, which caused bad audible whining, and 20 kHz, which
    // didn't work because the signal probably got too sloppy and
    // messed up the constant-current circuit. At 200 Hz we can
    // barely hear a low-pitched buzz.
    Prescaler #(.SCALE(250),
                .SCALE_BITS(8)) pwm_prescaler(
        .clock(clock_50),
        .reset_n(reset_n),
        .tick(pwm_tick)
    );
    PWM #(.MAX_VALUE(PWM_MAX_VALUE),
          .BITS(PWM_BITS)) pwm(
        .clock(clock_50),
        .reset_n(reset_n),
        .tick(pwm_tick),
        .value(pwm_value),
        .signal(lcd_pwm)
    );

endmodule
