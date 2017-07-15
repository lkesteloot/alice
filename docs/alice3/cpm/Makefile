ASM =	../../alice2/brads_alice2_archive/asm/asm80
.SUFFIXES: .hex .asm
.PHONY: all

.asm.hex:
	@echo 'Assembling "$<" because of "$?"'
	${ASM} $<

all: cpm22.hex alice3_cbios.hex boot.dsk

cpm22.hex: cpm22.asm

alice3_cbios.hex: alice3_cbios.asm

cpm_loader.hex: cpm_loader.asm

putsys.hex: putsys.asm

boot.dsk: hex2dsk cpm_loader.hex cpm22.hex alice3_cbios.hex
	./hex2dsk --template ../../alice2/emulator/driveA.bin cpm_loader.hex cpm22.hex alice3_cbios.hex boot.dsk

hex2dsk: hex2dsk.c
	cc hex2dsk.c ../emulator/readhex.c -o hex2dsk
