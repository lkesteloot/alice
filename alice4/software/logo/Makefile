.PHONY: clean run

APP=logo
SRC=light.c logo.c 
LIBGL_SRC=../libgl/gl.c ../libgl/vector.c ../libgl/network.c

$(APP): $(SRC) light.h $(LIBGL_SRC)
	cc -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o $(APP) $(SRC) $(LIBGL_SRC) -lm

run: $(APP)
	./$(APP)

clean:
	rm -f $(APP)
