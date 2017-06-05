
.PHONY: all

all: hps2fpga_sdram1 hps2fpga_test1 memtool protocol lcd_backlight benchmark memthrash

protocol: protocol.cpp ../libgl/awesome.c ../libgl/awesome.h
	g++ -I../libgl protocol.cpp ../libgl/awesome.c -o protocol

benchmark: benchmark.cpp ../libgl/awesome.c ../libgl/awesome.h
	g++ -I../libgl benchmark.cpp ../libgl/awesome.c -o benchmark

