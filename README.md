# TFTGLCDAdapter



TFT GLCD Adapter was developed to work wiht Smoothieware board to replace old monochrome LCDs with low resolution fonts, but now it
works wit Marlin 1.1.9 to.
For use this adapter wiht Smoothieware board you need compile Smoothieware firmware from https://github.com/Serhiy-K/Smoothieware/tree/add_new_panel.
Then you need set "panel.enable" parameter in config file to "true",
 change "panel.lcd" parameter to "tft_glcd_adapter" and set
proper parameters for SPI bus.

TFT GLCD Adapter
 based on STM32F103C8T6 "Blue Pill" board and color TFT GLCD with ILI9325 chip with 320x240 resolution.
Also it has 1 encoder and 1 button, but you may set up to 
8 buttons for Smoothieware and up to 3 buttons for Marlin,
include encoder button.

For connection with Smoothieware FT GLCD Adapter uses SPI bus, for Marlin-1x - I2C bus.

Firmware created with CoIDE.

Hardware directory include pcb-project for this adapter created in Altium Designer and real fotos.

Discussion about this TFT GLCD Adapter and my realisation Smoothieboard you may find on https://www.radiokot.ru/forum/viewtopic.php?f=25&t=162580