#
# Makefile for jello and its button buttonfly
#
include	$(ROOT)/usr/include/make/commondefs

IDBTAG = -idb "std.sw.demos"
AREA = cpu
SALESIDB = -idb "demos.sw.${AREA}"
DATAIDB = -idb "demos.data.${AREA}"
JIDB = -idb "demos.journal.${AREA}"

TARGETS = jello
CFILES = jello.c

OPTIMIZER = -O

LLDLIBS = -lgl_s -lm

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
jello.o: $(INCLDIR)/device.h $(INCLDIR)/gl.h $(INCLDIR)/math.h \
	 $(INCLDIR)/sgimath.h $(INCLDIR)/stdio.h $(INCLDIR)/values.h \
	 jello.c
# DO NOT DELETE THIS 2nd LINE -- make depend uses it
