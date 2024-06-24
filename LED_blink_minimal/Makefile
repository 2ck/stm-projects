CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld

CFLAGS = -mcpu=cortex-m7
LDFLAGS = -T linker.ld

OPENOCD_FLAGS = -f interface/stlink.cfg -f target/stm32h7x.cfg

SRC = main.c startup.c
OBJ = $(SRC:.c=.o)

all: led_blink.elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

led_blink.elf: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) led_blink.elf

flash: led_blink.elf
	openocd $(OPENOCD_FLAGS) -c "program led_blink.elf reset exit"
