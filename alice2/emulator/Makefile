.PHONY: all
OPT=-g
LDFLAGS=-L/opt/local/lib -lvncserver -lreadline
CXXFLAGS=-I/opt/local/include -std=c++11 $(OPT)

all: emulator sendhex

emulator: emulator.o z80emu.o readhex.o
	$(CXX) $(LDFLAGS) $^   -o $@

sendhex: sendhex.c
	$(CC) sendhex.c -o sendhex

clean:
	rm emulator emulator.o z80emu.o readhex readhex.o

immaculate: clean
	rm tables.h maketables

emulator.o: emulator.h z80emu.h

z80emu.o: z80emu.cpp z80emu.h instructions.h macros.h tables.h emulator.h

readhex.o: readhex.c readhex.h

tables.h: maketables.c
	$(CC) -Wall $< -o maketables
	./maketables > $@

