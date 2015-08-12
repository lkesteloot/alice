LDFLAGS=-L/opt/local/lib -lvncserver
CXXFLAGS=-I/opt/local/include -std=c++11
CFLAGS = -Wall -ansi -pedantic -O2 -fomit-frame-pointer

simulator: simulator.o z80emu.o
	$(CXX) $(LDFLAGS) $^   -o $@

z80emu.o: z80emu.cpp z80emu.h instructions.h macros.h tables.h

#$(CC) $(CFLAGS) -c $<

tables.h: maketables.c
	$(CC) -Wall $< -o maketables
	./maketables > $@

