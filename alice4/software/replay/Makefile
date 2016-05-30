.PHONY: clean run

APP=replay
SRC=replay.c ../libgl/network.c

$(APP): $(SRC) 
	cc -g -I../sys_include -I../demo_include -I../libgl \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o $(APP) $(SRC)

clean:
	rm -f $(APP)
