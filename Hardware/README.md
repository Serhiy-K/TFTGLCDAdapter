# TFTGLCDAdapter

## LCD with 16-bit bus
By default TFT GLCD Adapter uses LCD with 8-bit data bus. To connect LCD with 16-bit data bus you need connect controller direct to D7..D0 LCD data bus and by additional 8-bit reg to D15..D8 LCD data bus:

* Controller A[7..0] => LCD DB[7..0]
* Controller A[7..0] => 8-Bit_REG Din[7..0], 8-Bit_REG Dout[7..0] => LCD DB[15..8]
* Controller C14 => 8-Bit_REG WR

## Connect to 5V boards
TFT GLCD Adapter developed to connect with main board by 3.3V SPI signals. If you want connect it to main board with 5V TTL-level signals (Arduino) you need add level converter for SPI input signals. This converter may be built on different schematic - with diodes and resistors or with microchips. On picture below you may see both variants. Also on this picture you may see level converter for I2C bus.

## Add SD support
If you want add SD-card support to TFT GLCD Adapter you need add multiplexer for MISO signal between panel MISO and SD-card MISO signals. This need because panel and SD-card share the same SPI bus. Two posible variants you may see on picture below.

<img src="./tft-glcd-add_SD.jpg" width="800" height="528">