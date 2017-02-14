## Generated SDC file "Alice4.sdc"

## Copyright (C) 1991-2013 Altera Corporation
## Your use of Altera Corporation's design tools, logic functions 
## and other software and tools, and its AMPP partner logic 
## functions, and any output files from any of the foregoing 
## (including device programming or simulation files), and any 
## associated documentation or information are expressly subject 
## to the terms and conditions of the Altera Program License 
## Subscription Agreement, Altera MegaCore Function License 
## Agreement, or other applicable license agreement, including, 
## without limitation, that your use is for the sole purpose of 
## programming logic devices manufactured by Altera and sold by 
## Altera or its authorized distributors.  Please refer to the 
## applicable agreement for further details.


## VENDOR  "Altera"
## PROGRAM "Quartus II"
## VERSION "Version 13.1.0 Build 162 10/23/2013 SJ Web Edition"

## DATE    "Mon Nov 28 02:52:26 2016"

##
## DEVICE  "EP3C16F484C6"
##


#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3



#**************************************************************
# Create Clock
#**************************************************************

create_clock -name {CLOCK_50} -period 20.000 -waveform { 0.000 10.000 } [get_ports {CLOCK_50}]
create_clock -name {CLOCK_50_2} -period 20.000 -waveform { 0.000 10.000 } [get_ports {CLOCK_50_2}]


#**************************************************************
# Create Generated Clock
#**************************************************************

create_generated_clock -name {sdram_clock|altpll_component|auto_generated|pll1|clk[0]} -source [get_pins {sdram_clock|altpll_component|auto_generated|pll1|inclk[0]}] -duty_cycle 50.000 -multiply_by 83 -divide_by 25 -master_clock {CLOCK_50} [get_pins {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}] 
create_generated_clock -name {vga_clock|altpll_component|auto_generated|pll1|clk[0]} -source [get_pins {vga_clock|altpll_component|auto_generated|pll1|inclk[0]}] -duty_cycle 50.000 -multiply_by 74 -divide_by 147 -master_clock {CLOCK_50_2} [get_pins {vga_clock|altpll_component|auto_generated|pll1|clk[0]}] 


#**************************************************************
# Set Clock Latency
#**************************************************************



#**************************************************************
# Set Clock Uncertainty
#**************************************************************

set_clock_uncertainty -rise_from [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}] -rise_to [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.020  
set_clock_uncertainty -rise_from [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}] -fall_to [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.020  
set_clock_uncertainty -rise_from [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}] -rise_to [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.150  
set_clock_uncertainty -rise_from [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}] -fall_to [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.150  
set_clock_uncertainty -fall_from [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}] -rise_to [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.020  
set_clock_uncertainty -fall_from [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}] -fall_to [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.020  
set_clock_uncertainty -fall_from [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}] -rise_to [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.150  
set_clock_uncertainty -fall_from [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}] -fall_to [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.150  
set_clock_uncertainty -rise_from [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}] -rise_to [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.150  
set_clock_uncertainty -rise_from [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}] -fall_to [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.150  
set_clock_uncertainty -rise_from [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}] -rise_to [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.020  
set_clock_uncertainty -rise_from [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}] -fall_to [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.020  
set_clock_uncertainty -fall_from [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}] -rise_to [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.150  
set_clock_uncertainty -fall_from [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}] -fall_to [get_clocks {vga_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.150  
set_clock_uncertainty -fall_from [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}] -rise_to [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.020  
set_clock_uncertainty -fall_from [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}] -fall_to [get_clocks {sdram_clock|altpll_component|auto_generated|pll1|clk[0]}]  0.020  


#**************************************************************
# Set Input Delay
#**************************************************************



#**************************************************************
# Set Output Delay
#**************************************************************



#**************************************************************
# Set Clock Groups
#**************************************************************



#**************************************************************
# Set False Path
#**************************************************************

set_false_path -from [get_keepers {*rdptr_g*}] -to [get_keepers {*ws_dgrp|dffpipe_id9:dffpipe16|dffe17a*}]
set_false_path -from [get_keepers {*delayed_wrptr_g*}] -to [get_keepers {*rs_dgwp|dffpipe_hd9:dffpipe12|dffe13a*}]
set_false_path -from [get_keepers {Vga_control:vga_control|oTopOfScreen}] -to [get_keepers {m_vga_top_of_screen_latched}]
set_false_path -from [get_keepers vga_fifo_clear]


#**************************************************************
# Set Multicycle Path
#**************************************************************



#**************************************************************
# Set Maximum Delay
#**************************************************************



#**************************************************************
# Set Minimum Delay
#**************************************************************



#**************************************************************
# Set Input Transition
#**************************************************************

