.PHONY: clean run

APP=replay
SRC=replay.c ../libgl/network.c

$(APP): $(SRC) 
	cc -O2 -g -I../sys_include -I../demo_include -I../libgl \
		-Wno-implicit-function-declaration -Wno-implicit-int -Wno-unused-value \
		-Wno-return-type \
		-o $(APP) $(SRC) -lm

clean:
	rm -f $(APP)
