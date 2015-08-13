LDFLAGS=-L/opt/local/lib -lvncserver
CXXFLAGS=-I/opt/local/include -std=c++11 -g

emulator: emulator.o z80emu.o
	$(CXX) $(LDFLAGS) $^   -o $@

emulator.o: emulator.h z80emu.h

z80emu.o: z80emu.cpp z80emu.h instructions.h macros.h tables.h emulator.h

tables.h: maketables.c
	$(CC) -Wall $< -o maketables
	./maketables > $@

