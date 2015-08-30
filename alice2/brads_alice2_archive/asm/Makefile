ASM =	asm80
.SUFFIXES: .hex .asm

all:	memtest.hex  shortmem.hex \
	walk4000.hex \
	sendhex rom.hex

.asm.hex:
	@echo 'Assembling "$<" because of "$?"'
	${ASM} $<


rom.hex: rom.asm library.asm stdio.asm string.asm zcc_out.asm libc.asm printf.asm gets.asm strcpy.asm

# grantham 20150829 We should figure out if we care about these
shortmem.hex: library.asm
walk4000.hex: library.asm

sendhex:    sendhex.c
	    cc -Wall sendhex.c -o sendhex
