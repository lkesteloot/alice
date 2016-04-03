FIRMWARE_VERSION    := $(shell date)

OPT             =       -O2 -g

TOOLROOT        =       $(HOME)/trees/gcc-arm-none-eabi-4_9-2015q3
STM32FCUBEROOT  =       $(HOME)/trees/STM32Cube_FW_F4_V1.9.0
GCC_LIB         =       $(TOOLROOT)/lib/gcc/arm-none-eabi/4.9.3

CORTEX_M4_HWFP_CC_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian
TARGET = STM32F415xx
STM32F4XX_CMSIS_INC_PATH        = $(STM32FCUBEROOT)/Drivers/CMSIS/Include/
CORTEX_M4_HWFP_LIB_PATH = $(GCC_LIB)/armv7e-m/fpu

OBJECTS = \
        STM32Cube4/startup_stm32f417xx.o \
        STM32Cube4/system_stm32f4xx.o \
        STM32Cube4/stm32f4xx_it.o \
        STM32Cube4/stm32f4xx_hal_cortex.o \
        STM32Cube4/stm32f4xx_hal_dma.o \
        STM32Cube4/stm32f4xx_hal_gpio.o \
        STM32Cube4/stm32f4xx_hal_rcc.o \
        STM32Cube4/stm32f4xx_hal_uart.o \
        STM32Cube4/stm32f4xx_hal_spi.o \
        STM32Cube4/stm32f4xx_hal.o \
        syscalls.o \
        ff.o \
        delay.o \
        leds.o \
        monitor_queue.o \
        console_queue.o \
        ps2_keyboard.o \
        uart.o \
        utility.o \
        crc7.o \
        logprintf.o \
        sd_spi.o \
        $(NULL)

SOURCES = $(OBJECTS:.o=.c)
DEPS = $(OBJECTS:.o=.d)

ioboard.hex: ioboard.elf
	$(TOOLROOT)/bin/arm-none-eabi-objcopy -O ihex $< $@

-include $(DEPS)

ioboard.elf:  $(OBJECTS) ioboard.o
	$(TOOLROOT)/bin/arm-none-eabi-gcc $(OPT) $(CORTEX_M4_HWFP_CC_FLAGS) -DSTM32F415xx -L$(CORTEX_M4_HWFP_LIB_PATH) -TSTM32F415RG_FLASH.ld -lm -specs=nosys.specs -Wl,--gc-sections $^ -o $@

STM32Cube4/startup_stm32f417xx.o: STM32Cube4/startup_stm32f417xx.s
	$(TOOLROOT)/bin/arm-none-eabi-gcc -Wall $(OPT) $(CORTEX_M4_HWFP_CC_FLAGS)  $< -c -o $@

%.o: %.c
	$(TOOLROOT)/bin/arm-none-eabi-gcc -std=c99 -Wall $(OPT) -ISTM32Cube4 -DIOBOARD_FIRMWARE_VERSION="$(FIRMWARE_VERSION)" -fno-builtin -I$(STM32F4XX_CMSIS_INC_PATH) -D$(TARGET) $(CORTEX_M4_HWFP_CC_FLAGS)  $< -c -o $@ -MMD

%.s: %.c
	$(TOOLROOT)/bin/arm-none-eabi-gcc -S -std=c99 -Wall $(OPT) -ISTM32Cube4 -DIOBOARD_FIRMWARE_VERSION="$(FIRMWARE_VERSION)" -fno-builtin -I$(STM32F4XX_CMSIS_INC_PATH) -D$(TARGET) $(CORTEX_M4_HWFP_CC_FLAGS)  $< -c -o $@

clean:
	rm -f *.hex *.elf *.o $(OBJECTS) $(DEPS)

bootrom.bin: ../../rom/rom.bin
	(cd ../../rom ; make rom.bin)
	dd bs=1024 count=1 if=../../rom/rom.bin of=bootrom.bin

ioboard.dfu: ioboard.hex
	python dfu-convert -i ioboard.hex ioboard.dfu

burn: ioboard.dfu
	sudo ~/trees/dfu-util/src/dfu-util -d 0483:df11 -a 0 -D ioboard.dfu
