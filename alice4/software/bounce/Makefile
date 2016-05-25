.PHONY: clean run

APP=bounce
SRC=bounce.c 
LIBGL_SRC=../libgl/gl.c ../libgl/vector.c ../libgl/network.c

$(APP): $(SRC) $(LIBGL_SRC)
	cc -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o $(APP) $(SRC) $(LIBGL_SRC)

run: $(APP)
	./$(APP)

clean:
	rm -f $(APP)
