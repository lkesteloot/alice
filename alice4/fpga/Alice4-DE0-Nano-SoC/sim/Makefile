
.PHONY: run verilate build

VERILATOR_ROOT=$(HOME)

# System test
MAIN=Main
MAIN_CPP=sim_main

# FIFO test
## MAIN=Fifo_test
## MAIN_CPP=fifo_test_main

run: build
	./obj_dir/V$(MAIN)

build: verilate
	# Add OPT=-DVL_DEBUG to display debug output.
	cd obj_dir && make -j -f V$(MAIN).mk V$(MAIN)

verilate:
	verilator -Wall --cc $(MAIN).v --exe $(MAIN_CPP).cpp

out.gif:
	convert -delay 1x50 -loop 1 out-*.ppm out.gif
