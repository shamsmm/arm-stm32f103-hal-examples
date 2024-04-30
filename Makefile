CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

# List of object target files to compile, includes project and libraries
OBJS=startup_stm32f103.o syscalls.o main.o stm32f1xx_it.o system_stm32f1xx.o stm32f1xx_hal.o stm32f1xx_hal_cortex.o stm32f1xx_hal_rcc.o stm32f1xx_hal_gpio.o stm32f1xx_hal_dma.o stm32f1xx_hal_uart.o stm32f1xx_hal_i2c.o

BUILD=build
SRC=src

STM32CubeF1=/home/shams/W/Embedded/ARM/Libraries/STM32CubeF1
STM32F1XX_HAL_DRIVER=$(STM32CubeF1)/Drivers/STM32F1xx_HAL_Driver
CMSIS=$(STM32CubeF1)/Drivers/CMSIS

# Paths to libraries .c source files
vpath %.c $(STM32F1XX_HAL_DRIVER)/Src

# Paths to project .c source files
vpath %.c $(SRC)

CFLAGS=-ggdb -mthumb -mcpu=cortex-m3 -O2 -DSTM32F103xB -DUSE_FULL_ASSERT

# Paths to libraries .h header files
CFLAGS+= -I$(STM32F1XX_HAL_DRIVER)/Inc/ -I$(CMSIS)/Device/ST/STM32F1xx/Include/ -I$(CMSIS)/Include

# Include this project header files
CFLAGS+= -Iinc/

# Auto create build directory
_ := $(shell mkdir -p $(BUILD))

#flash: main.bin
#	st-flash --reset write main.bin 0x08000000

#main.bin: main.elf
	#$(OBJCOPY) -O binary -j .text -j .rodata -j .ARM -j preinit_array -j init_array -j fini_array -j .data main.elf main.bin

flash: main.elf
	openocd -f stlink.cfg -c "program main.elf verify reset exit"

main.elf: $(OBJS)
	$(CC) $(CFLAGS) -TSTM32F103.ld -o main.elf $(BUILD)/*

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $(BUILD)/$@

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $(BUILD)/$@

clean:
	rm -f build/*