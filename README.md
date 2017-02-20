#Matrix digital clock



DIY clock with these features:
- LED matrix - 16 x 64 (1024 pixels) or version with 16 x 96 pixels
- DCF Receiver and time synchronization from the radio
- or ESP8826 Wifi module and time synchronization from a NTP server

Project page: https://hackaday.io/project/19949-led-matrix-clock

![photo](photos/20150624_203145.jpg)

Hardware
-----

The hardware consist of these PCBs:
- 2 or 3 pcs LED matrices (each one 16 x 32 LEDs)
- controller board with STM32F100 MCU and shift registers

Firmware
----

Firmware is based on [ChibiOS](http://www.chibios.org) embedded operating system

Build
----

Development toolchain is GNU arm gcc (https://launchpadlibrarian.net/209776202/gcc-arm-none-eabi-4_9-2015q2-20150609-linux.tar.bz2) version 4.9 - it contains interesting newlib library which saves a lot of RAM and hardware floating point support and many other features.

You can flash the MCU via openocd or my [kstlink](https://github.com/kubanecxxx/kstlink) project which is simple gdb server to debug and flash STM microcontrollers (I feel openocd too heavy).
