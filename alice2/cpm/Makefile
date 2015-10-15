ASM =	../brads_alice2_archive/asm/asm80
.SUFFIXES: .hex .asm
.PHONY: all

.asm.hex:
	@echo 'Assembling "$<" because of "$?"'
	${ASM} $<

all: cpm22.hex fakehw_bios.hex

cpm22.hex: cpm22.asm

fakehw_bios.hex: fakehw_bios.asm

putsys.hex: putsys.asm
