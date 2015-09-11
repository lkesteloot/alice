.PHONY: all

BG80D_PATH=$(HOME)/trees/bg80d
USE_BG80D=1

OPT=-g
LDFLAGS=-L/opt/local/lib -lvncserver -lreadline
CXXFLAGS=-Wall -I/opt/local/include -I$(BG80D_PATH) -DUSE_BG80D=$(USE_BG80D) -std=c++11 $(OPT)

VPATH=$(BG80D_PATH)

all: emulator sendhex

emulator: emulator.o z80emu.o readhex.o
	$(CXX) $(LDFLAGS) $^   -o $@

sendhex: sendhex.c
	$(CC) sendhex.c -o sendhex

clean:
	rm emulator emulator.o z80emu.o readhex readhex.o

immaculate: clean
	rm tables.h maketables

emulator.o: emulator.h z80emu.h bg80d.h

z80emu.o: z80emu.cpp z80emu.h instructions.h macros.h tables.h emulator.h

readhex.o: readhex.c readhex.h

tables.h: maketables.c
	$(CC) -Wall $< -o maketables
	./maketables > $@

