
module fb_fifo (
	clk_clk,
	reset_reset_n,
	fifo_0_in_writedata,
	fifo_0_in_write,
	fifo_0_in_waitrequest,
	fifo_0_out_readdata,
	fifo_0_out_read,
	fifo_0_out_waitrequest);	

	input		clk_clk;
	input		reset_reset_n;
	input	[63:0]	fifo_0_in_writedata;
	input		fifo_0_in_write;
	output		fifo_0_in_waitrequest;
	output	[63:0]	fifo_0_out_readdata;
	input		fifo_0_out_read;
	output		fifo_0_out_waitrequest;
endmodule
