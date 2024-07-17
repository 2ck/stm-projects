# Example Projects for the NUCLEO-H753ZI

## Getting Started

> :warning: **Note:** These instructions assume you are using Linux. Everything
> should still work on MacOS or with WSL, but it hasn't been tested there.

### Dependencies

You can find the necessary dependencies for each project in its README.

If you don't want to deal with that yourself, you can [install
Nix](https://nixos.org/download/) and [enable
flakes](https://nixos.wiki/wiki/Flakes).

Then you can simply run

```
nix develop
```

and you will be launched into a shell with all the necessary dependencies.

### Additional Setup

For flashing the board via USB (and access to the serial console) you may need
to change USB permissions. If you use WSL, the [WSL USB
Manager](https://gitlab.com/alelec/wsl-usb-gui) can do that for you, in addition
to making the USB devices available inside WSL.

Otherwise, you may need to manually add [udev
rules](https://wiki.archlinux.org/title/udev) in `/etc/udev/rules.d/`, e.g.:

```
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374d", MODE="660", GROUP="dialout", SYMLINK+="stlinkv3loader_%n"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374e", MODE="660", GROUP="dialout", SYMLINK+="stlinkv3_%n"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374f", MODE="660", GROUP="dialout", SYMLINK+="stlinkv3_%n"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3753", MODE="660", GROUP="dialout", SYMLINK+="stlinkv3_%n"
```

With the following commands, you can check if everything is correctly applied.

```
$ lsusb
[...]
Bus 005 Device 009: ID 0483:374e STMicroelectronics STLINK-V3
[...]
$ ls -l /dev/bus/usb/005/009
crw-rw---- 1 root dialout [...]
$ groups
dialout [...]
```

## Folder Structure

[Drivers/](./Drivers) contains CMSIS, HAL and BSP. They can be found in the
[STM32CubeH7 repository](https://github.com/STMicroelectronics/STM32CubeH7).

[LED_blink_minimal/](./LED_blink_minimal) is a minimal-dependency program that
blinks an LED on the board.

[LED_blink/](./LED_blink) implements LED blinking with the HAL and BSP drivers.

[UART_echo/](./UART_echo) allows the board to receive data via USB serial and
echo it back out to the sender.

[util/](./util) contains a Python script for communication over serial ports,
and some example images.
