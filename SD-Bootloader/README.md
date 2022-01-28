# SD-Bootloader for STM32F103

This is the SD-card bootloader for STM32F103C8(CB) chips. It based on Fystec STM32F103 bootloader.

## Build bootloader

You may finde prebuilded binary and HEX files in `SDLoader\Debug\bin directory`. This project uses simple Makefile to build the project and may use VS-Code as IDE. Upload loader to MCU by SWD interface.

## Rebuild main firmware

This bootloader default boot address is 0x08003800 (14k), so if you want to use this bootloader you need to rebuild the main firmware. Before building you need uncomment "USE_SD_BOOTLOADER" line first in **Makefile** for main firmware.

## Upload main firmware with sdcard

After you upload the bootloader and rebuild the main firmware, now you can upload it to the panel with sdcard

1. Power off the machine.
2. Copy the main firmware to the SD card, you need to be sure the firmware name is "3D-GLCD.bin". 
3. Insert the SD card to the panel.
4. Power on the machine.
5. And wait for about 3 seconds, it will be done.
