.PHONY: clean

APP=buttonfly
SRC=buttonfly.c event.c lexer.c parser.tab.c
HEADERS=bartfont.h buttonfly.h data.h event.h y.tab.h
LIBGL_SRC=../libgl/gl.c ../libgl/network.c

$(APP): $(SRC) $(HEADERS) $(LIBGL_SRC)
	cc -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o $(APP) $(SRC) $(LIBGL_SRC)

parser.tab.c: parser.y
	bison parser.y

clean:
	rm -f $(APP) parser.tab.c

