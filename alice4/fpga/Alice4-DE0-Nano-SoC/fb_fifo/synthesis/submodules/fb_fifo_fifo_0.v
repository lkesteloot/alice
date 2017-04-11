//Legal Notice: (C)2017 Altera Corporation. All rights reserved.  Your
//use of Altera Corporation's design tools, logic functions and other
//software and tools, and its AMPP partner logic functions, and any
//output files any of the foregoing (including device programming or
//simulation files), and any associated documentation or information are
//expressly subject to the terms and conditions of the Altera Program
//License Subscription Agreement or other applicable license agreement,
//including, without limitation, that your use is for the sole purpose
//of programming logic devices manufactured by Altera and sold by Altera
//or its authorized distributors.  Please refer to the applicable
//agreement for further details.

// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

// turn off superfluous verilog processor warnings 
// altera message_level Level1 
// altera message_off 10034 10035 10036 10037 10230 10240 10030 

module fb_fifo_fifo_0_single_clock_fifo (
                                          // inputs:
                                           aclr,
                                           clock,
                                           data,
                                           rdreq,
                                           wrreq,

                                          // outputs:
                                           empty,
                                           full,
                                           q
                                        )
;

  output           empty;
  output           full;
  output  [ 63: 0] q;
  input            aclr;
  input            clock;
  input   [ 63: 0] data;
  input            rdreq;
  input            wrreq;


wire             empty;
wire             full;
wire    [ 63: 0] q;
  scfifo single_clock_fifo
    (
      .aclr (aclr),
      .clock (clock),
      .data (data),
      .empty (empty),
      .full (full),
      .q (q),
      .rdreq (rdreq),
      .wrreq (wrreq)
    );

  defparam single_clock_fifo.add_ram_output_register = "OFF",
           single_clock_fifo.intended_device_family = "CYCLONEV",
           single_clock_fifo.lpm_numwords = 32,
           single_clock_fifo.lpm_showahead = "OFF",
           single_clock_fifo.lpm_type = "scfifo",
           single_clock_fifo.lpm_width = 64,
           single_clock_fifo.lpm_widthu = 5,
           single_clock_fifo.overflow_checking = "ON",
           single_clock_fifo.underflow_checking = "ON",
           single_clock_fifo.use_eab = "ON";


endmodule


// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

// turn off superfluous verilog processor warnings 
// altera message_level Level1 
// altera message_off 10034 10035 10036 10037 10230 10240 10030 

module fb_fifo_fifo_0_scfifo_with_controls (
                                             // inputs:
                                              clock,
                                              data,
                                              rdreq,
                                              reset_n,
                                              wrreq,

                                             // outputs:
                                              empty,
                                              full,
                                              q
                                           )
;

  output           empty;
  output           full;
  output  [ 63: 0] q;
  input            clock;
  input   [ 63: 0] data;
  input            rdreq;
  input            reset_n;
  input            wrreq;


wire             empty;
wire             full;
wire    [ 63: 0] q;
wire             wrreq_valid;
  //the_scfifo, which is an e_instance
  fb_fifo_fifo_0_single_clock_fifo the_scfifo
    (
      .aclr  (~reset_n),
      .clock (clock),
      .data  (data),
      .empty (empty),
      .full  (full),
      .q     (q),
      .rdreq (rdreq),
      .wrreq (wrreq_valid)
    );

  assign wrreq_valid = wrreq & ~full;

endmodule


// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

// turn off superfluous verilog processor warnings 
// altera message_level Level1 
// altera message_off 10034 10035 10036 10037 10230 10240 10030 

module fb_fifo_fifo_0 (
                        // inputs:
                         avalonmm_read_slave_read,
                         avalonmm_write_slave_write,
                         avalonmm_write_slave_writedata,
                         reset_n,
                         wrclock,

                        // outputs:
                         avalonmm_read_slave_readdata,
                         avalonmm_read_slave_waitrequest,
                         avalonmm_write_slave_waitrequest
                      )
;

  output  [ 63: 0] avalonmm_read_slave_readdata;
  output           avalonmm_read_slave_waitrequest;
  output           avalonmm_write_slave_waitrequest;
  input            avalonmm_read_slave_read;
  input            avalonmm_write_slave_write;
  input   [ 63: 0] avalonmm_write_slave_writedata;
  input            reset_n;
  input            wrclock;


wire    [ 63: 0] avalonmm_read_slave_readdata;
wire             avalonmm_read_slave_waitrequest;
wire             avalonmm_write_slave_waitrequest;
wire             clock;
wire    [ 63: 0] data;
wire             empty;
wire             full;
wire    [ 63: 0] q;
wire             rdreq;
wire             wrreq;
  //the_scfifo_with_controls, which is an e_instance
  fb_fifo_fifo_0_scfifo_with_controls the_scfifo_with_controls
    (
      .clock   (clock),
      .data    (data),
      .empty   (empty),
      .full    (full),
      .q       (q),
      .rdreq   (rdreq),
      .reset_n (reset_n),
      .wrreq   (wrreq)
    );

  //in, which is an e_avalon_slave
  //out, which is an e_avalon_slave
  assign data = avalonmm_write_slave_writedata;
  assign wrreq = avalonmm_write_slave_write;
  assign avalonmm_read_slave_readdata = q;
  assign rdreq = avalonmm_read_slave_read;
  assign clock = wrclock;
  assign avalonmm_write_slave_waitrequest = full;
  assign avalonmm_read_slave_waitrequest = empty;

endmodule

