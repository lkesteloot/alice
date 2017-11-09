ASM =	zasm
HEX2BIN =	../../alice3/emulator/hex2bin
.SUFFIXES: .hex .asm
.PHONY: all burn

.asm.hex:
	@echo 'Assembling "$<" because of "$?"'
	${ASM} -w -x $< -l . -o .

all: basic.hex

basic.hex: basic.asm

basic.bin: basic.hex
	$(HEX2BIN) basic.hex basic.bin
