FAMILY          =       pic16
COMPILE_DEVICE  =       18f452
BURN_DEVICE     =       pic18f452

io_pic.hex: io_pic.c
	/Applications/microchip/xc8/v1.35/bin/xc8 --chip=18F452 io_pic.c

# .c.o:
	# sdcc --use-non-free -m$(FAMILY) -p$(COMPILE_DEVICE) -c $<

# io_pic.hex: io_pic.o
	# gplink -r -w -m -o io_pic.hex io_pic.o /usr/local/share/sdcc/lib/$(FAMILY)/libsdcc.lib /usr/local/share/sdcc/non-free/lib/$(FAMILY)/libdev$(COMPILE_DEVICE).lib

# Need to figure out toolchain sequence for partial compilation; for now just go right from C to HEX
# io_pic.hex: io_pic.c
	# sdcc -DFAMILY=$(FAMILY) -DSDCC_MODE=1 --use-non-free -m$(FAMILY) -p$(COMPILE_DEVICE) io_pic.c

clean:
	rm io_pic.o io_pic.asm

burn:
	echo extracting binary chunks to write...
	../emulator/hex2bin -r 256 0x300000 io_pic.hex io_pic.config.bin
	../emulator/hex2bin -r 32768 0 io_pic.hex io_pic.code.bin
	echo
	echo writing to device...
	minipro -p $(BURN_DEVICE) -c code -w io_pic.code.bin
	minipro -p $(BURN_DEVICE) -e -c data -w io_pic.config.bin

sanityburn:
	echo extracting binary chunks to write...
	../emulator/hex2bin -r 256 0x300000 sanity.hex sanity.config.bin
	../emulator/hex2bin -r 32768 0 sanity.hex sanity.code.bin
	echo
	echo writing to device...
	minipro -p $(BURN_DEVICE) -c code -w sanity.code.bin
	minipro -p $(BURN_DEVICE) -e -c data -w sanity.config.bin

burn16C65:
	echo extracting binary chunks to write...
	../emulator/hex2bin -r 16384 0 ../io_pic_v2/io_pic.hex ../io_pic_v2/io_pic.code.bin
	echo
	echo writing to device...
	minipro -p PIC16C65A -c code -w io_pic.code.bin

# xc8 --clist --chip=18F452 io_pic.c
