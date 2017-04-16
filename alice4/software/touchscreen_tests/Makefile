CXXFLAGS	+=	-g -Wall -I../libgl
LDLIBS	+=	-lvncserver

all: ts_test1 vncts

ts_test1: ts_test1.cpp ../libgl/touchscreen.c

vncts: vncts.cpp ../libgl/touchscreen.c
