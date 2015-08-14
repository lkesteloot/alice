OPT=-O2
LDFLAGS=-L/opt/local/lib -lvncserver
CXXFLAGS=-I/opt/local/include -std=c++11 $(OPT)

emulator: emulator.o z80emu.o
	$(CXX) $(LDFLAGS) $^   -o $@

clean:
	rm emulator emulator.o z80emu.o

immaculate: clean
	rm tables.h maketables

emulator.o: emulator.h z80emu.h

z80emu.o: z80emu.cpp z80emu.h instructions.h macros.h tables.h emulator.h

tables.h: maketables.c
	$(CC) -Wall $< -o maketables
	./maketables > $@

