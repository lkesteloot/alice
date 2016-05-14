.PHONY: clean

LIBGL_SRC=../libgl/gl.c ../libgl/network.c

insect: insect.c insect.h mat.c parts.c $(LIBGL_SRC)
	cc -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o insect insect.c mat.c parts.c $(LIBGL_SRC)

clean:
	if [ -f insect ]; then rm insect; fi
