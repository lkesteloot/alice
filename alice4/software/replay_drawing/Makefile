all: replay_drawing_reference replay_drawing_network

replay_drawing_network: replay_drawing.cpp ../libgl/network_rasterizer.c ../libgl/network.c
	gcc -DNDEBUG -O2 -I../sys_include -I../libgl replay_drawing.cpp ../libgl/network_rasterizer.c ../libgl/network.c -o replay_drawing_network

replay_drawing_reference: replay_drawing.cpp ../libgl/reference_rasterizer.c ../libgl/network.c
	gcc -DNDEBUG -O2 -I../sys_include -I../libgl replay_drawing.cpp ../libgl/reference_rasterizer.c ../libgl/network.c -o replay_drawing_reference
