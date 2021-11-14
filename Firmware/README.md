# Firmware for TFTGLCDAdapter

Firmware for TFT GLCD Adapter was created with CoIDE but may be built wiht simple Makefile. Also you may use VSCode as IDE.

Read and change **inc/defines.h** to set proper parameters.

## Main parameters
* `HW_VER_1`                     - panel uses "BluePill" board, encoder and LCD with 8 bit data bus
* `HW_VER_2`                     - panel uses own PCB, encoder, micro-SD card and LCD with 16 bit data bus
* `HW_VER_3`                     - panel uses resistive touchscreen instead encoder and LCD with 8 bit data bus
* `CALIBR_DEBUG_INFO`            - outputs old and new ADC values during touchscreen calibration
* `INVERT_ENCODER_DIR`           - change direction for encoder
* `SET_ORIENT_RIGHT (LEFT)`      - set LCD orientation by software
* `ILIxxxx`, `ST7789`, `HX8347`, `R61509V` - LCD chip. Select only one of them
* `LCD_BRIGHTNES`                - initial brightnes for LCD
* `WITHOUT_HEAT_ICO`             - if you want see "FAN %" text insted heat icon on 320x240 screen
* `USE_3DPRINTER`, `USE_LASER`   - you may select only one of this variants to reduce ROM size or both for universality
* `ONLY_MARLIN`                  - support only Marlin to reduce ROM size
* `USE_SPI`, `USE_I2C`           - you may select only one of this variants to reduce ROM size or both for universality
* `FONT`                         - font for text. Courier_New_Bold or LiberationMono
* `BUZ_FREQ_MP`                  - buzzer input frequency multiplier. Change to get more suitable tones
* `LOGO_FREQx`                   - buzzer frequency for start logo
* `I2C_Panel_Addr`               - I2C addres, is used only for Marlin and **must be equal to LCD_I2C_ADDRESS parameter in Conditionals_LCD.h**