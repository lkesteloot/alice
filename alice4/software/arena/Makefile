.PHONY: clean run

APP=arena
SRC=arena.c colision.c colors.c comm.c damage.c def.c explode.c hud.c main.c maze.c move.c objects.c shot.c startup.c udpbrdcst.c utils.c
LIBGL_SRC=../libgl/gl.c ../libgl/vector.c ../libgl/network.c

$(APP): $(SRC) arena.h udpbrdcst.h $(LIBGL_SRC)
	cc -std=c99 -g -I../sys_include -I../demo_include \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-DNETWORKING=1 \
		-o $(APP) $(SRC) $(LIBGL_SRC)

run: $(APP)
	./$(APP)

clean:
	rm -f $(APP)


