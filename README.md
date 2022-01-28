# TFT GLCD Panel
TFT GLCD Panel was developed as low cost replacement for monochrome LCDs with low resolution fonts. It works with Smoothieware, Marlin-1 and Marlin-2.

<img src="./Hardware/Photos/Smoothie-progressbar.jpg"  width="265" height="200">

**TFT GLCD Panel "Version 1.0"** based on STM32F103CBT6 "Blue Pill" board and color TFT GLCD on ILI9325, ILI9341, HX8347 and ST7789 chips with 320x240 resolution or ILI9327 and R61509V chips with 400x240 resolution. Also it has 1 encoder and 1 button, but you may set up to 6 buttons for Smoothieware and up to 3 buttons for Marlin, include encoder button. Controller connected to LCD by 8-bit data bus.

**TFT GLCD Panel "Version 2.0"** based on the same STM32F103CBT6 controller, but has own PCB. It desighned to connect LCD and controller by 16-bit data bus. Also this panel has standart EXP2 connector for connect to printer's main board and integrated Micro-SD connector.

**TFT GLCD Panel "Version 2.1"** equal to "Version 2.0", but may use SD-Bootloader for firmware upgrading by SD-card. First you must write SD-Bootloader to the MCU and firmware file "3D-GLCD.bin" will be uploaded to MCU from SD-card. To enter in boot mode you need insert SD-card with "3D-GLCD.bin" file before turning on power. After uploading firmware "3D-GLCD.bin" file will be deleted.

**TFT GLCD Panel "Version 3.0"** was designed to add resistive touchscreen support. Touchscreen is connected to controller's pinns directly. Controller connected to LCD by 8-bit data bus. This is wery simple realisation without SD support but SD support may be added as for "Version 2.0".

Firmware for TFT GLCD Panel created with CoIDE but may be built wiht simple Makefile. Also you may use VSCode as IDE. GCC for ARM is used as a compiler.

Hardware directory include pcb-projects for this panel created in Altium Designer, gerber files and real photos.

Discussion about this TFT GLCD Panel and my realization Smoothieboard you may find on https://www.radiokot.ru/forum/viewtopic.php?f=25&t=162580

## Using
To connect with Smoothieware TFT GLCD Panel uses only SPI bus, with Marlin - SPI or I2C buses.

For use this panel with Smoothieware you need:
* Compile Smoothieware firmware from https://github.com/Serhiy-K/Smoothieware/tree/add_new_panel.
* Set `panel.enable` parameter in **config** file to `true`.
* Change `panel.lcd` parameter in **config** file to `tft_glcd_panel`.
* Set proper parameters for SPI bus.

For use this panel with Marlin you need:
* For Marlin-1 add/replace your Marlin's files with files from archive in "For-Marlin" directory.
* For Marlin-2 you need get latest code from https://github.com/MarlinFirmware/Marlin. You may use stabble 2.0.x tree or bugfix-2.0.x.
* Read **Readme.txt** in "For-Marlin" directory for more details.
* Edit Configuration.h.
* Check and edit section with text `TFTGLCD_PANEL` in **Conditionals_LCD.h** and recompile Marlin firmware.

## Connect to 5V boards
TFT GLCD Panel "Version 1.0" developed to connect with main board by 3.3V SPI signals. If you want connect it to main board with 5V TTL-level signals (Arduino) **you need add level converter** for SPI **input** signals. A level converter is not needed for the MISO signal. This converter may be built on different schematic - with diodes and resistors or with microchips. On picture below you may see both variants. For use I2C bus **you need remove pull-up resistors betwin I2C lines and +5V on main board** if they present. TFT GLCD Panel "Version 2.0" already has level converter for SPI input signals.

## Add SD support to TFT GLCD Panel "Version 1.0" and "Version 3.0"
If you want add SD-card support you need add multiplexer for MISO signal between panel's and SD-card's MISO signals. This need because panel and SD-card share the same SPI bus. Two posible variants you may see on picture below and in TFT GLCD Panel "Version 2" project. Also you may change panel connector for "Version 1.0" to standart EXP2 pinout.

<img src="./Hardware/tft-glcd-add_SD.jpg" width="800" height="528">