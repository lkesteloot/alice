.PHONY: all
OPT=-O2
LDFLAGS=-L/opt/local/lib -lvncserver
CXXFLAGS=-I/opt/local/include -std=c++11 $(OPT)

all: emulator sendhex hex2bin

emulator: emulator.o z80emu.o readhex.o
	$(CXX) $(LDFLAGS) $^   -o $@

sendhex: sendhex.c
	$(CC) sendhex.c -o sendhex

hex2bin: hex2bin.o readhex.o
	$(CC) hex2bin.o readhex.o -o hex2bin

clean:
	rm emulator emulator.o z80emu.o readhex readhex.o

immaculate: clean
	rm tables.h maketables

emulator.o: emulator.h z80emu.h

z80emu.o: z80emu.cpp z80emu.h instructions.h macros.h tables.h emulator.h

readhex.o: readhex.c readhex.h

hex2bin.o: hex2bin.c readhex.h

tables.h: maketables.c
	$(CC) -Wall $< -o maketables
	./maketables > $@

