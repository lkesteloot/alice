.PHONY: clean run

APP=bounce
SRC=bounce.c 
LIBGL_SRC=../libgl/gl.c ../libgl/vector.c ../libgl/network.c ../libgl/network_rasterizer.c ../libgl/network_events.c

$(APP): $(SRC) $(LIBGL_SRC)
	cc -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o $(APP) $(SRC) $(LIBGL_SRC) -lm

run: $(APP)
	./$(APP) ./logo.bin

clean:
	rm -f $(APP)
