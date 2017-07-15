
.PHONY: all

# C_FILES = $(wildcard *.c)
# ASM_FILES = locore.s
C_FILES = tst.c

ALL_ASM_FILES = $(C_FILES:.c=.s) $(ASM_FILES)
SINGLE_ASM_FILE = zcc_out.s
CC = ../zcc/zcc
AS = ../brads_alice2_archive/asm/az80/az80

all:	$(SINGLE_ASM_FILE)

#zcc_out.hex: $(SINGLE_ASM_FILE)
#	$(AS) $< -o $(basename $<).hex -l $(basename $<).prn

$(SINGLE_ASM_FILE): $(ALL_ASM_FILES)
	cat $(ALL_ASM_FILES) > $@

%.s: %.c $(CC)
	$(CC) $<

# Include dependencies. Update manually.
kern.o: kern.h proc.h locore.h
proc.o: proc.h mem.h
main.o: kern.h proc.h mem.h
mem.o: mem.h
