
//`define enable_ADC
//`define enable_ARDUINO
//`define enable_GPIO0
//`define enable_GPIO1
//`define enable_HPS

module Main(
        //////////// CLOCK //////////
        input clock_50,
        input clock2_50,
        input clock3_50,

`ifdef enable_ADC
        //////////// ADC //////////
        /* 3.3-V LVTTL */
        output adc_convst,
        output adc_sclk,
        output adc_sdi,
        input adc_sdo,
`endif
        
`ifdef enable_ARDUINO
        //////////// ARDUINO ////////////
        /* 3.3-V LVTTL */
        inout [15:0] arduino_io,
        inout arduino_reset_n,
`endif
        
`ifdef enable_GPIO0
        //////////// GPIO 0 ////////////
        /* 3.3-V LVTTL */
        inout [35:0] gpio_0,
`endif

`ifdef enable_GPIO1     
        //////////// GPIO 1 ////////////
        /* 3.3-V LVTTL */
        inout [35:0] gpio_1,
`endif

`ifdef enable_HPS
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

reg [23:0] counter;

always @(posedge clock_50) begin
   counter <= counter + 1;
end

assign led[0] = counter[23];

endmodule
