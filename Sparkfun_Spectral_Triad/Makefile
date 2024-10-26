CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy

# basic compiler flags
CFLAGS = -mcpu=cortex-m7 -mthumb -O0 -lc -lm
# debug information
CFLAGS += -g
# enable HAL (hardware abstraction library)
CFLAGS += -DSTM32H753xx -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER
# optimize resulting binary
CFLAGS += -fdata-sections -ffunction-sections -Wl,--gc-sections
LDFLAGS = -T STM32H743ZITX_FLASH.ld

INCLUDES =  -IInc -I../Drivers/BSP/STM32H7xx_Nucleo
INCLUDES += -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include
INCLUDES += -I../Drivers/STM32H7xx_HAL_Driver/Inc
INCLUDES += -Isparkfun

SOURCES = main.c system_stm32h7xx.c
SOURCES +=  ../Drivers/BSP/STM32H7xx_Nucleo/stm32h7xx_nucleo.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_exti.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_utils.c \
			../Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_rcc.c
SOURCES += sparkfun/SparkFun_AS7265X.c sparkfun/i2c_stub.c
OBJECTS = $(SOURCES:.c=.o) startup_stm32h753xx.o

OPENOCD_FLAGS = -f interface/stlink.cfg -f target/stm32h7x.cfg 

PROJECT_NAME = spectroscope
all: $(PROJECT_NAME).bin

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.s
	$(AS) $< -o $@

$(PROJECT_NAME).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(PROJECT_NAME).bin: $(PROJECT_NAME).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OBJECTS) $(PROJECT_NAME).elf $(PROJECT_NAME).bin

# when flashing .bin, the start address is needed, because
# unlike the .elf, it does not contain that information
flash: $(PROJECT_NAME).bin
	openocd $(OPENOCD_FLAGS) -c "program $(PROJECT_NAME).bin 0x08000000 verify reset exit"

gdbserver:
	openocd $(OPENOCD_FLAGS)

# opens minicom for UART/serial input/output
serial:
	minicom --device /dev/ttyACM0 --baudrate 115200
