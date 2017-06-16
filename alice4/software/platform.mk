
UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
	# NOTE put libgl/network.c in only once
	RASTERIZER_SRCS = ../libgl/network_rasterizer.c ../libgl/network.c
	EVENTS_SRCS = ../libgl/network_events.c # ../libgl/network.c
else
	RASTERIZER_SRCS = ../libgl/hardware_rasterizer.c ../libgl/awesome.c
	EVENTS_SRCS = ../libgl/hardware_events.c ../libgl/touchscreen.c ../libgl/i2c_utility.c ../libgl/accelerometer.c	
endif

# RASTERIZER_SRCS = ../libgl/reference_rasterizer.c
# EVENTS_SRCS = ../libgl/network_events.c ../libgl/network.c
# EVENTS_SRCS = ../libgl/hardware_events.c ../libgl/touchscreen.c

