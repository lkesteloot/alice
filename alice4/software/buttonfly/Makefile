# 
# This makefile creates the buttonfly menu and installs it into /usr/demos
#
include	${ROOT}/usr/include/make/commondefs

IDBTAG = -idb "std.sw.demos"
GEOIDB = -idb "demos.sw.geo"
BUTTONS = tools color clock

LCINCS =	-I. -I../../include
LLDLIBS =	-lgl_s -lm
OPTIMIZER =	-O

TARGETS = buttonfly
CFILES = buttonfly.c lexer.c event.c
YFILES = parser.y
YFLAGS = -d
LDIRT = y.tab.h
HFILES = bartfont.h buttonfly.h data.h event.h

default: ${TARGETS}

include ${COMMONRULES}

install: default
	${INSTALL} -F /usr/demos -m 755 -u demos -g demos \
		${IDBTAG} ${TARGETS}
	${INSTALL} -F /usr/demos -m 755 -u demos -g demos \
		${IDBTAG} buttonfly.info
	${INSTALL} -F /usr/demos/bin -m 755 -u demos -g demos \
		${IDBTAG} eye.ps
	${INSTALL} -F /usr/demos -m 644 -u demos -g demos \
		${IDBTAG} -src menu_demos .menu_demos
	${INSTALL} -F /usr/demos -m 644 -u demos -g demos \
		${IDBTAG} -src menu .menu
	${INSTALL} -F /usr/demos -m 644 -u demos -g demos \
		${IDBTAG} -src m_showimg .m_image_showimg
	for i in ${BUTTONS}; do ( \
		${INSTALL} -F /usr/demos -m 644 -u demos -g demos \
			${IDBTAG} -src m_$$i .m_$$i ; \
	) done;

sales: install
	${INSTALL} -F /usr/demos -m 644 -u demos -g demos -src m_geo\
		${GEOIDB} .m_applications_geo

${TARGETS}:	${OBJECTS}
	${CCF} ${OBJECTS} ${LDFLAGS} -o $@

y.tab.h: ${YFILES:.y=.o}

# DO NOT DELETE THIS LINE -- make depend uses it
buttonfly.o: $(INCLDIR)/device.h $(INCLDIR)/gl.h $(INCLDIR)/math.h \
	 $(INCLDIR)/sgimath.h $(INCLDIR)/stdio.h $(INCLDIR)/values.h \
	 bartfont.h buttonfly.c buttonfly.h data.h event.h
event.o: $(INCLDIR)/device.h $(INCLDIR)/stdio.h event.c event.h
lexer.o: $(INCLDIR)/stdio.h buttonfly.h lexer.c y.tab.h
parser.o: $(INCLDIR)/stdio.h buttonfly.h parser.y
# DO NOT DELETE THIS 2nd LINE -- make depend uses it
