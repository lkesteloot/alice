
/* verilator lint_off UNDRIVEN */
module soc_system (
	clk_clk,
	hps_0_f2h_sdram0_data_address,
	hps_0_f2h_sdram0_data_burstcount,
	hps_0_f2h_sdram0_data_waitrequest,
	hps_0_f2h_sdram0_data_readdata,
	hps_0_f2h_sdram0_data_readdatavalid,
	hps_0_f2h_sdram0_data_read,
	hps_0_f2h_sdram1_data_address,
	hps_0_f2h_sdram1_data_burstcount,
	hps_0_f2h_sdram1_data_waitrequest,
	hps_0_f2h_sdram1_data_readdata,
	hps_0_f2h_sdram1_data_readdatavalid,
	hps_0_f2h_sdram1_data_read,
	hps_0_f2h_sdram2_data_address,
	hps_0_f2h_sdram2_data_burstcount,
	hps_0_f2h_sdram2_data_waitrequest,
	hps_0_f2h_sdram2_data_readdata,
	hps_0_f2h_sdram2_data_readdatavalid,
	hps_0_f2h_sdram2_data_read,
	hps_0_f2h_sdram3_data_address,
	hps_0_f2h_sdram3_data_burstcount,
	hps_0_f2h_sdram3_data_waitrequest,
	hps_0_f2h_sdram3_data_writedata,
	hps_0_f2h_sdram3_data_byteenable,
	hps_0_f2h_sdram3_data_write,
	hps_0_f2h_sdram4_data_address,
	hps_0_f2h_sdram4_data_burstcount,
	hps_0_f2h_sdram4_data_waitrequest,
	hps_0_f2h_sdram4_data_writedata,
	hps_0_f2h_sdram4_data_byteenable,
	hps_0_f2h_sdram4_data_write,
	memory_mem_a,
	memory_mem_ba,
	memory_mem_ck,
	memory_mem_ck_n,
	memory_mem_cke,
	memory_mem_cs_n,
	memory_mem_ras_n,
	memory_mem_cas_n,
	memory_mem_we_n,
	memory_mem_reset_n,
	memory_mem_dq,
	memory_mem_dqs,
	memory_mem_dqs_n,
	memory_mem_odt,
	memory_mem_dm,
	memory_oct_rzqin,
	reset_reset_n,
	hps_0_i2c1_scl_in_clk,
	hps_0_i2c1_clk_clk,
	hps_0_i2c1_out_data,
	hps_0_i2c1_sda);	

	input		clk_clk /* verilator public */;
	input	[28:0]	hps_0_f2h_sdram0_data_address /* verilator public */;
	input	[7:0]	hps_0_f2h_sdram0_data_burstcount /* verilator public */;
	output		hps_0_f2h_sdram0_data_waitrequest /* verilator public */;
	output	[63:0]	hps_0_f2h_sdram0_data_readdata /* verilator public */;
	output		hps_0_f2h_sdram0_data_readdatavalid /* verilator public */;
	input		hps_0_f2h_sdram0_data_read /* verilator public */;
	input	[28:0]	hps_0_f2h_sdram1_data_address /* verilator public */;
	input	[7:0]	hps_0_f2h_sdram1_data_burstcount /* verilator public */;
	output		hps_0_f2h_sdram1_data_waitrequest /* verilator public */;
	output	[63:0]	hps_0_f2h_sdram1_data_readdata /* verilator public */;
	output		hps_0_f2h_sdram1_data_readdatavalid /* verilator public */;
	input		hps_0_f2h_sdram1_data_read /* verilator public */;
	input	[28:0]	hps_0_f2h_sdram2_data_address /* verilator public */;
	input	[7:0]	hps_0_f2h_sdram2_data_burstcount /* verilator public */;
	output		hps_0_f2h_sdram2_data_waitrequest /* verilator public */;
	output	[63:0]	hps_0_f2h_sdram2_data_readdata /* verilator public */;
	output		hps_0_f2h_sdram2_data_readdatavalid /* verilator public */;
	input		hps_0_f2h_sdram2_data_read /* verilator public */;
	input	[28:0]	hps_0_f2h_sdram3_data_address /* verilator public */;
	input	[7:0]	hps_0_f2h_sdram3_data_burstcount /* verilator public */;
	output		hps_0_f2h_sdram3_data_waitrequest /* verilator public */;
	input	[63:0]	hps_0_f2h_sdram3_data_writedata /* verilator public */;
	input	[7:0]	hps_0_f2h_sdram3_data_byteenable /* verilator public */;
	input		hps_0_f2h_sdram3_data_write /* verilator public */;
	input	[28:0]	hps_0_f2h_sdram4_data_address /* verilator public */;
	input	[7:0]	hps_0_f2h_sdram4_data_burstcount /* verilator public */;
	output		hps_0_f2h_sdram4_data_waitrequest /* verilator public */;
	input	[63:0]	hps_0_f2h_sdram4_data_writedata /* verilator public */;
	input	[7:0]	hps_0_f2h_sdram4_data_byteenable /* verilator public */;
	input		hps_0_f2h_sdram4_data_write /* verilator public */;
	output	[14:0]	memory_mem_a /* verilator public */;
	output	[2:0]	memory_mem_ba /* verilator public */;
	output		memory_mem_ck /* verilator public */;
	output		memory_mem_ck_n /* verilator public */;
	output		memory_mem_cke /* verilator public */;
	output		memory_mem_cs_n /* verilator public */;
	output		memory_mem_ras_n /* verilator public */;
	output		memory_mem_cas_n /* verilator public */;
	output		memory_mem_we_n /* verilator public */;
	output		memory_mem_reset_n /* verilator public */;
	inout	[31:0]	memory_mem_dq /* verilator public */;
	inout	[3:0]	memory_mem_dqs /* verilator public */;
	inout	[3:0]	memory_mem_dqs_n /* verilator public */;
	output		memory_mem_odt /* verilator public */;
	output	[3:0]	memory_mem_dm /* verilator public */;
	input		memory_oct_rzqin /* verilator public */;
	input		reset_reset_n /* verilator public */;
	input		hps_0_i2c1_scl_in_clk /* verilator public */;
	output		hps_0_i2c1_clk_clk /* verilator public */;
	output		hps_0_i2c1_out_data /* verilator public */;
	input		hps_0_i2c1_sda /* verilator public */;
endmodule
/* verilator lint_on UNDRIVEN */
