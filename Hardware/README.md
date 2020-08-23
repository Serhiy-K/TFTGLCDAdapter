# TFTGLCDAdapter

## LCD with 8-bit bus
For this data bus use TFT GLCD Adapter "Version 1.0". You may build own PCB without "Blue Pill" board.

## LCD with 16-bit bus
For this data bus use TFT GLCD Adapter "Version 2.0".

## Connect to 5V boards
TFT GLCD Adapter "Version 1.0" developed to connect with printer's main board by 3.3V SPI signals. If you want connect it to main board with 5V TTL-level signals (Arduino) you need add levels converter for SPI input signals. This converter may be built on different schematic - with diodes and resistors or with microchips. On picture below you may see both variants. For use I2C bus you need remove pull-up resistors betwin I2C lines and +5V on main board if they present. TFT GLCD Adapter "Version 2.0" already has level converter for SPI input signals.

## Add SD support to TFT GLCD Adapter "Version 1.0"
If you want add SD-card support you need add multiplexer for MISO signal between panel's and SD-card's MISO signals. This need because panel and SD-card share the same SPI bus. Two posible variants you may see on picture below and in TFT GLCD Adapter "Version 2.0" project. Also you may change panel connector to standart EXP2 pinout.

<img src="./tft-glcd-add_SD.jpg" width="800" height="528">