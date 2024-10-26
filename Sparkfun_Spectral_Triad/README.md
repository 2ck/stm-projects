# Sparkfun AS7265x Spectral Triad

This code takes a Spectral Triad sensor measurement when/while the user push
button on the board is pressed (at most every second). It then prints the value
of several channels to the serial console via UART.

After getting the necessary dependencies, power the sensor with 3.3V and connect
its I2C pins to I2C2 on your nucleo board. SDA and SCL are D68/PF0 and D69/PF1,
respectively. Then connect your board via USB and run

```
make flash
```

After flashing successfully, connect to the board's serial port with

```
make serial
```

You should then see output every time you press the user push button.

## Dependencies

- Sparkfun AS7265x breakout board
- Arm embedded toolchain
- OpenOCD
- GNU make
- minicom
- STM32H7 Drivers
