
ASM =	/home/grantham/bin/asm80
.SUFFIXES: .hex .asm


all:	birth.hex inttest.hex keytest.hex  \
	memtest.hex sctest.hex shortmem.hex \
	rom2.hex walk4000.hex pictest.hex \
	sendhex rom.hex

.asm.hex:
	@echo 'Assembling "$<" because of "$?"'
	${ASM} $<
	cp $*.hex /dosc/eprom/alice2


birth.hex: library.asm
inttest.hex: library.asm
pictest.hex: library.asm stdio.asm string.asm
rom.hex: rom.asm library.asm stdio.asm string.asm zcc_out.asm libc.asm printf.asm gets.asm strcpy.asm
keytest.hex: library.asm
memtest.hex: library.asm
sctest.hex: library.asm
shortmem.hex: library.asm
rom2.hex: library.asm
walk4000.hex: library.asm

sendhex:    sendhex.c
	    cc -Wall sendhex.c -o sendhex
