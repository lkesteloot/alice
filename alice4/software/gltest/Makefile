.PHONY: clean

SRC=gltest.c
LIBGL_SRC=../libgl/gl.c ../libgl/network.c

gltest: $(SRC) $(LIBGL_SRC)
	cc -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o gltest $(SRC) $(LIBGL_SRC)

clean:
	if [ -f gltest ]; then rm gltest; fi
