.PHONY: clean

SRC=light.c logo.c 
LIBGL_SRC=../libgl/gl.c ../libgl/network.c

logo: $(SRC) light.h $(LIBGL_SRC)
	cc -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o logo $(SRC) $(LIBGL_SRC)

clean:
	if [ -f logo ]; then rm logo; fi
