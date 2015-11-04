ASM =	../../alice2/brads_alice2_archive/asm/asm80
.SUFFIXES: .hex .asm
.PHONY: all

.asm.hex:
	@echo 'Assembling "$<" because of "$?"'
	${ASM} $<

all: rom.hex

rom.hex: rom.asm loader_address.asm loader_data.asm

loader.hex: loader.asm loader_address.asm

loader_data.asm: loader.hex hex2asm
	# Sync 0x8000 with loader_address.asm:
	./hex2asm ldrdat ldrcnt 0x8000 < loader.hex > loader_data.asm

hex2asm: hex2asm.c
	cc hex2asm.c ../emulator/readhex.c -o hex2asm

