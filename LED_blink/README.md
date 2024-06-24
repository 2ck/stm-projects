# Controlling LEDs with HAL+BSP

This code blinks all three user LEDs on the board through HAL and BSP functions.

After getting the necessary dependencies, connect your board via USB and run

```
make flash
```

## Dependencies

- Arm embedded toolchain
- OpenOCD
- GNU make

## File structure

[Drivers/](./Drivers) contains CMSIS, HAL and BSP. They can be found in the
[STM32CubeH7 repository](https://github.com/STMicroelectronics/STM32CubeH7).

[Inc/](./Inc) contains headers for configuring [HAL](./Inc/stm32h7xx_hal_conf.h)
and [BSP](./Inc/stm32h7xx_nucleo_conf.h). Templates for these files are found in
[Drivers/STM32H7xx_HAL_Driver/Inc/](./Drivers/STM32H7xx_HAL_Driver/Inc/) and
[Drivers/BSP/STM32H7xx_Nucleo/](./Drivers/BSP/STM32H7xx_Nucleo/), respectively.

[main.c](./main.c) contains the main code, which toggles the LEDs in a loop.

[startup_stm32h753xx.s](./startup_stm32h753xx.s) is the startup file, whose code
is run directly after power on. It calls the system init function (and main). It
is provided as a template by CMSIS
[here](./Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc/).

[system_stm32h7xx.c](./system_stm32h7xx.c) performs system initialization steps,
and contains clock configuration.

[STM32H753ZITX_FLASH.ld](./STM32H753ZITX_FLASH.ld) is a linker script generated
for the STM32H753 by the CubeIDE.

[Makefile](./Makefile) defines targets for building and flashing the code.
