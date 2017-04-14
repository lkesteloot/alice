.PHONY: clean

APP=insect
SRC=insect.c mat.c parts.c
LIBGL_SRC=../libgl/gl.c ../libgl/vector.c ../libgl/network.c ../libgl/network_rasterizer.c ../libgl/network_events.c

$(APP): $(SRC) insect.h $(LIBGL_SRC)
	cc -O2 -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o $(APP) $(SRC) $(LIBGL_SRC) -lm

run: $(APP)
	./$(APP)

clean:
	rm -f $(APP)
