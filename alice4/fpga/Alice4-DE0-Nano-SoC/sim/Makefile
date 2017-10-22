
.PHONY: run verilate build

VERILATOR_ROOT=$(HOME)

run: build
	./obj_dir/VMain

build: verilate
	cd obj_dir && make -j -f VMain.mk VMain

verilate:
	verilator -Wall --cc Main.v --exe sim_main.cpp
