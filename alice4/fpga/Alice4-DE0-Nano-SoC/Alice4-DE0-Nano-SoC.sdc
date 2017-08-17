create_clock -name "clock_50" -period 20ns [get_ports {clock_50}]

# For enhancing USB BlasterII to be reliable, 25MHz. (Got this from System Builder generated file. -LK)
create_clock -name {altera_reserved_tck} -period 40 {altera_reserved_tck}
set_input_delay -clock altera_reserved_tck -clock_fall 3 [get_ports altera_reserved_tdi]
set_input_delay -clock altera_reserved_tck -clock_fall 3 [get_ports altera_reserved_tms]
set_output_delay -clock altera_reserved_tck 3 [get_ports altera_reserved_tdo]

derive_clock_uncertainty
