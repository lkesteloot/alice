.PHONY: clean

SRC=buttonfly.c event.c lexer.c
HEADERS=bartfont.h buttonfly.h data.h event.h y.tab.h
LIBGL_SRC=../libgl/gl.c ../libgl/network.c

insect: $(SRC) $(HEADERS) $(LIBGL_SRC)
	cc -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o insect $(SRC) $(LIBGL_SRC)

clean:
	if [ -f insect ]; then rm insect; fi

