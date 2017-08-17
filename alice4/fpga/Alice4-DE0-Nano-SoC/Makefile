
.PHONY: all Prescaler_tb_run

IVERILOG_BIN=$(HOME)/local/bin

all: main

main: *.v
	$(IVERILOG_BIN)/iverilog -s Main -o main *.v

Prescaler_tb: Prescaler*.v
	$(IVERILOG_BIN)/iverilog -o Prescaler_tb Prescaler*.v


Prescaler_tb_run: Prescaler_tb
	$(IVERILOG_BIN)/vvp Prescaler_tb



