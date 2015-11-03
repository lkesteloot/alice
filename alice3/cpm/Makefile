ASM =	../../alice2/brads_alice2_archive/asm/asm80
.SUFFIXES: .hex .asm
.PHONY: all

.asm.hex:
	@echo 'Assembling "$<" because of "$?"'
	${ASM} $<

all: cpm22.hex alice3_cbios.hex

cpm22.hex: cpm22.asm

alice3_cbios.hex: alice3_cbios.asm

putsys.hex: putsys.asm
