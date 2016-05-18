# 
# This makefile creates the arena demo and installs it into /usr/demos
#
include ${ROOT}/usr/include/make/commondefs
include ${PRODUCTDEFS}

IDBTAG = -idb "std.sw.demos"
AREA = graphics
SALESIDB = -idb "demos.sw.${AREA}"
DATAIDB = -idb "demos.data.${AREA}"
JIDB = -idb "demos.journal.${AREA}"

TARGETS = arena
CFILES = arena.c colision.c colors.c damage.c def.c explode.c hud.c maze.c \
	move.c objects.c shot.c startup.c main.c utils.c comm.c udpbrdcst.c
HFILES = arena.h udpbrdcst.h

LCOPTS = -float
LCDEFS = -DCLOVER
LLDLIBS =	-lgl_s -lsun -lbsd -lm
OPTIMIZER =	-O

#STANDARD DEMO MAKEFILE

default: ${TARGETS}

include ${COMMONRULES}

${TARGETS}: ${OBJECTS}
	${CCF} -o $@ ${OBJECTS} ${LDFLAGS}

# Install demo, buttonfly file, information slide, and shell script
# for environment manager
install: default
	${INSTALL} -F /usr/demos/bin -m 755 -u demos -g demos \
		${IDBTAG} ${TARGETS}
	for i in ${TARGETS}; do ( \
		${INSTALL} -F /usr/demos -m 644 -u demos -g demos \
			${IDBTAG} -src m_$$i .m_${AREA}_$$i ;\
		${INSTALL} -F /usr/demos/${AREA} -m 755 -u demos -g demos \
			${IDBTAG} -src $$i.sh $$i ;\
		${INSTALL} -F /usr/demos/${AREA} -m 755 -u demos -g demos \
			${IDBTAG} $$i.info ;\
	) done;

# Install everything install does under different idb's, and
# install journalling stuff.
sales: default
	${INSTALL} -F /usr/demos/bin -m 755 -u demos -g demos \
		${SALESIDB} ${TARGETS}
	for i in ${TARGETS}; do ( \
		${INSTALL} -F /usr/demos -m 644 -u demos -g demos \
			${SALESIDB} -src m_$$i .m_${AREA}_$$i ;\
		${INSTALL} -F /usr/demos/${AREA} -m 755 -u demos -g demos \
			${SALESIDB} -src $$i.sh $$i ;\
		${INSTALL} -F /usr/demos/${AREA} -m 755 -u demos -g demos \
			${SALESIDB} $$i.info ;\
		${INSTALL} -F /usr/demos/data/journal -m 644 -u demos -g demos \
			${JIDB} $$i.j ;\
		${INSTALL} -F /usr/demos/${AREA} -m 755 -u demos -g demos \
			${JIDB} $$i.rec;\
		${INSTALL} -F /usr/demos -m 644 -u demos -g demos \
			${JIDB} -src m_$${i}.j .m_${AREA}_$${i}.j ;\
	) done;

# DO NOT DELETE THIS LINE -- make depend uses it
arena.o: $(INCLDIR)/gl.h $(INCLDIR)/stdio.h arena.c arena.h
colision.o: $(INCLDIR)/device.h $(INCLDIR)/gl.h $(INCLDIR)/math.h \
	 $(INCLDIR)/sgimath.h $(INCLDIR)/stdio.h $(INCLDIR)/values.h \
	 arena.h colision.c
colors.o: $(INCLDIR)/gl.h arena.h colors.c
comm.o: $(INCLDIR)/bsd/netdb.h $(INCLDIR)/bsd/netinet/in.h \
	 $(INCLDIR)/bsd/sys/socket.h $(INCLDIR)/device.h \
	 $(INCLDIR)/errno.h $(INCLDIR)/gl.h $(INCLDIR)/stdio.h \
	 $(INCLDIR)/sys/errno.h $(INCLDIR)/sys/ioctl.h \
	 $(INCLDIR)/sys/termio.h $(INCLDIR)/sys/times.h \
	 $(INCLDIR)/sys/types.h arena.h comm.c udpbrdcst.h
damage.o: $(INCLDIR)/device.h $(INCLDIR)/gl.h arena.h damage.c
def.o: $(INCLDIR)/gl.h arena.h def.c
explode.o: $(INCLDIR)/gl.h arena.h explode.c
hud.o: $(INCLDIR)/gl.h arena.h hud.c
main.o: $(INCLDIR)/device.h $(INCLDIR)/gl.h $(INCLDIR)/stdio.h \
	 $(INCLDIR)/sys/times.h $(INCLDIR)/sys/types.h arena.h main.c
maze.o: $(INCLDIR)/gl.h $(INCLDIR)/stdio.h arena.h maze.c
move.o: $(INCLDIR)/gl.h $(INCLDIR)/math.h $(INCLDIR)/sgimath.h \
	 $(INCLDIR)/values.h arena.h move.c
objects.o: $(INCLDIR)/gl.h $(INCLDIR)/stdio.h arena.h objects.c
shot.o: $(INCLDIR)/device.h $(INCLDIR)/gl.h $(INCLDIR)/math.h \
	 $(INCLDIR)/sgimath.h $(INCLDIR)/stdio.h $(INCLDIR)/values.h \
	 arena.h shot.c
startup.o: $(INCLDIR)/device.h $(INCLDIR)/gl.h arena.h startup.c
udpbrdcst.o: $(INCLDIR)/bsd/netdb.h $(INCLDIR)/bsd/netinet/in.h \
	 $(INCLDIR)/bsd/sys/socket.h $(INCLDIR)/errno.h \
	 $(INCLDIR)/sys/errno.h $(INCLDIR)/sys/ioctl.h \
	 $(INCLDIR)/sys/termio.h $(INCLDIR)/sys/time.h \
	 $(INCLDIR)/sys/types.h $(INCLDIR)/time.h udpbrdcst.c
utils.o: $(INCLDIR)/device.h $(INCLDIR)/gl.h $(INCLDIR)/math.h \
	 $(INCLDIR)/sgimath.h $(INCLDIR)/values.h arena.h utils.c
# DO NOT DELETE THIS 2nd LINE -- make depend uses it
