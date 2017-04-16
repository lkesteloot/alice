	fb_fifo u0 (
		.clk_clk                (<connected-to-clk_clk>),                //        clk.clk
		.reset_reset_n          (<connected-to-reset_reset_n>),          //      reset.reset_n
		.fifo_0_in_writedata    (<connected-to-fifo_0_in_writedata>),    //  fifo_0_in.writedata
		.fifo_0_in_write        (<connected-to-fifo_0_in_write>),        //           .write
		.fifo_0_in_waitrequest  (<connected-to-fifo_0_in_waitrequest>),  //           .waitrequest
		.fifo_0_out_readdata    (<connected-to-fifo_0_out_readdata>),    // fifo_0_out.readdata
		.fifo_0_out_read        (<connected-to-fifo_0_out_read>),        //           .read
		.fifo_0_out_waitrequest (<connected-to-fifo_0_out_waitrequest>)  //           .waitrequest
	);

