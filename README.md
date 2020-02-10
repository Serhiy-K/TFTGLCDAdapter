# TFTGLCDAdapter

TFT GLCD Adapter was developed to work with Smoothieware board to replace old monochrome LCDs with low resolution fonts, but now it works with Marlin 1.1.9 and Marlin 2.0.x.

To connect with Smoothieware TFT GLCD Adapter uses only SPI bus, for Marlin - SPI or I2C bus.

TFT GLCD Adapter based on STM32F103C8T6 "Blue Pill" board and color TFT GLCD on ILI9325 chip with 320x240 resolution. Also it has 1 encoder and 1 button, but you may set up to 6 buttons for Smoothieware and up to 3 buttons for Marlin, include encoder button.

<img src="./Hardware/Photos/Smoothie-progressbar.jpg"/>

Firmware for TFT GLCD Adapter created with CoIDE.

Discussion about this TFT GLCD Adapter and my realization Smoothieboard you may find on https://www.radiokot.ru/forum/viewtopic.php?f=25&t=162580

For use this adapter with Smoothieware you need compile Smoothieware firmware from https://github.com/Serhiy-K/Smoothieware/tree/add_new_panel
Then you need set "panel.enable" parameter in config file to "true", change "panel.lcd" parameter to "tft_glcd_adapter" and set proper parameters for SPI bus.

For use this adapter with Marlin you need add/replace your Marlin's files with files from suitable archive in "For-Marlin" directory, check TFTGLCD_ADAPTER section in Conditionals_LCD, and recompile firmware.
Read Readme.txt in "For-Marlin" directory for more details.

Hardware directory include pcb-project for this adapter created in Altium Designer and real photos.

