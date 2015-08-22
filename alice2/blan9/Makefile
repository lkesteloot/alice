

OBJECTS = kern.o proc.o main.o locore.o mem.o
CFLAGS = -Wall -g
LDFLAGS = -g

all:	blan9

blan9:	$(OBJECTS)
	cc $(LDFLAGS) $(OBJECTS) -o blan9

locore.o: locore.s
	as locore.s -o locore.o

depend:
	makedepend $(OBJECTS:.o=.c)

# DO NOT DELETE

kern.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
kern.o: /usr/include/unistd.h /usr/include/features.h
kern.o: /usr/include/sys/cdefs.h /usr/include/posix_opt.h
kern.o: /usr/include/gnu/types.h
kern.o: /usr/src/gcc-2.5.8/lib/gcc-lib/i486-linux/2.5.8/include/stddef.h
kern.o: /usr/include/confname.h /usr/include/sys/types.h
kern.o: /usr/include/linux/types.h /usr/include/asm/types.h kern.h proc.h
kern.o: locore.h
proc.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
proc.o: /usr/include/malloc.h /usr/include/features.h
proc.o: /usr/include/sys/cdefs.h
proc.o: /usr/src/gcc-2.5.8/lib/gcc-lib/i486-linux/2.5.8/include/stddef.h
proc.o: proc.h
main.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
main.o: /usr/include/unistd.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/posix_opt.h
main.o: /usr/include/gnu/types.h
main.o: /usr/src/gcc-2.5.8/lib/gcc-lib/i486-linux/2.5.8/include/stddef.h
main.o: /usr/include/confname.h /usr/include/sys/types.h
main.o: /usr/include/linux/types.h /usr/include/asm/types.h
main.o: /usr/include/signal.h /usr/include/linux/signal.h kern.h proc.h
mem.o: /usr/include/malloc.h /usr/include/features.h /usr/include/sys/cdefs.h
mem.o: /usr/src/gcc-2.5.8/lib/gcc-lib/i486-linux/2.5.8/include/stddef.h mem.h
