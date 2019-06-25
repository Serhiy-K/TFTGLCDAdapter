# TFTGLCDAdapter



TFT GLCD Adapter developed to work wiht Smoothieware board to replace old monochrome LCDs with low resolution fonts.
For use this adapter wiht Smoothieware board you need compile Smoothieware firmware from https://github.com/Serhiy-K/Smoothieware.git.
Then you need set "panel.enable" parameter in config file to "true",
 * change "panel.lcd" parameter to "tft_glcd_adapter" and set
proper parameters for SPI bus.

TFT GLCD Adapter
 based on STM32F103C8T6 "Blue Pill" board and color TFT GLCD with ILI9325 chip with 320x240 resolution.
Also it has 1 encoder and 1 button, but you may set up to 
8 buttons, include encoder button.
Firmware created with CoIDE.

Hardware directory include pcb-project for this adapter created in Altium Designer.