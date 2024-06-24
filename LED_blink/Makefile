CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy

# basic compiler flags
CFLAGS = -mcpu=cortex-m7 -mthumb -O2 -lc -lm
# debug information
CFLAGS += -g
# enable HAL (hardware abstraction library)
CFLAGS += -DSTM32H753xx -DUSE_HAL_DRIVER
# optimize resulting binary
CFLAGS += -fdata-sections -ffunction-sections -Wl,--gc-sections
LDFLAGS = -T STM32H753ZITX_FLASH.ld

INCLUDES =  -IInc -IDrivers/BSP/STM32H7xx_Nucleo
INCLUDES += -IDrivers/CMSIS/Include -IDrivers/CMSIS/Device/ST/STM32H7xx/Include
INCLUDES += -IDrivers/STM32H7xx_HAL_Driver/Inc

OPENOCD_FLAGS = -f interface/stlink.cfg -f target/stm32h7x.cfg

SOURCES = main.c system_stm32h7xx.c
SOURCES +=  Drivers/BSP/STM32H7xx_Nucleo/stm32h7xx_nucleo.c \
			Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c \
			Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c \
			Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c \
			Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c \
			Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c
OBJECTS = $(SOURCES:.c=.o) startup_stm32h753xx.o

PROJECT_NAME = hal_demo
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
