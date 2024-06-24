# Minimal code to blink an LED

This code blinks LD1 on the board through memory-mapped registers.

After getting the necessary dependencies, connect your board via USB and run

```
make flash
```

## Dependencies

- Arm embedded toolchain
- OpenOCD
- GNU make

## File structure

[main.c](./main.c) contains the main code, which configures GPIO and toggles the
LED in a loop.

[startup.c](./startup.c) creates a minimal vector table, that the [linker
script](./linker.ld) puts at the start of flash memory. This makes a pointer to
the main function the reset vector, which the CPU jumps to on power up.

[linker.ld](./linker.ld) defines a minimal linker script for our
microcontroller. It sets the memory ranges for RAM and flash, and where each ELF
section will end up. Additionally, it defines the initial stack pointer (used in
[the startup code](./startup.c)).


[Makefile](./Makefile) defines targets for building and flashing the code.
