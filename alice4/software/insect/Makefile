.PHONY: clean

SRC=insect.c mat.c parts.c
LIBGL_SRC=../libgl/gl.c ../libgl/network.c

insect: $(SRC) insect.h $(LIBGL_SRC)
	cc -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o insect $(SRC) $(LIBGL_SRC)

clean:
	if [ -f insect ]; then rm insect; fi
