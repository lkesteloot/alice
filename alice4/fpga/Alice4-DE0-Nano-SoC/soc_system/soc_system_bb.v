
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
	hps_0_f2h_sdram4_data_address,
	hps_0_f2h_sdram4_data_burstcount,
	hps_0_f2h_sdram4_data_waitrequest,
	hps_0_f2h_sdram4_data_writedata,
	hps_0_f2h_sdram4_data_byteenable,
	hps_0_f2h_sdram4_data_write);	

	input		clk_clk;
	input	[28:0]	hps_0_f2h_sdram0_data_address;
	input	[7:0]	hps_0_f2h_sdram0_data_burstcount;
	output		hps_0_f2h_sdram0_data_waitrequest;
	output	[63:0]	hps_0_f2h_sdram0_data_readdata;
	output		hps_0_f2h_sdram0_data_readdatavalid;
	input		hps_0_f2h_sdram0_data_read;
	input	[28:0]	hps_0_f2h_sdram1_data_address;
	input	[7:0]	hps_0_f2h_sdram1_data_burstcount;
	output		hps_0_f2h_sdram1_data_waitrequest;
	output	[63:0]	hps_0_f2h_sdram1_data_readdata;
	output		hps_0_f2h_sdram1_data_readdatavalid;
	input		hps_0_f2h_sdram1_data_read;
	input	[28:0]	hps_0_f2h_sdram2_data_address;
	input	[7:0]	hps_0_f2h_sdram2_data_burstcount;
	output		hps_0_f2h_sdram2_data_waitrequest;
	output	[63:0]	hps_0_f2h_sdram2_data_readdata;
	output		hps_0_f2h_sdram2_data_readdatavalid;
	input		hps_0_f2h_sdram2_data_read;
	input	[28:0]	hps_0_f2h_sdram3_data_address;
	input	[7:0]	hps_0_f2h_sdram3_data_burstcount;
	output		hps_0_f2h_sdram3_data_waitrequest;
	input	[63:0]	hps_0_f2h_sdram3_data_writedata;
	input	[7:0]	hps_0_f2h_sdram3_data_byteenable;
	input		hps_0_f2h_sdram3_data_write;
	output	[14:0]	memory_mem_a;
	output	[2:0]	memory_mem_ba;
	output		memory_mem_ck;
	output		memory_mem_ck_n;
	output		memory_mem_cke;
	output		memory_mem_cs_n;
	output		memory_mem_ras_n;
	output		memory_mem_cas_n;
	output		memory_mem_we_n;
	output		memory_mem_reset_n;
	inout	[31:0]	memory_mem_dq;
	inout	[3:0]	memory_mem_dqs;
	inout	[3:0]	memory_mem_dqs_n;
	output		memory_mem_odt;
	output	[3:0]	memory_mem_dm;
	input		memory_oct_rzqin;
	input		reset_reset_n;
	input	[28:0]	hps_0_f2h_sdram4_data_address;
	input	[7:0]	hps_0_f2h_sdram4_data_burstcount;
	output		hps_0_f2h_sdram4_data_waitrequest;
	input	[63:0]	hps_0_f2h_sdram4_data_writedata;
	input	[7:0]	hps_0_f2h_sdram4_data_byteenable;
	input		hps_0_f2h_sdram4_data_write;
endmodule
