# Firmware for TFTGLCDAdapter

Firmware for TFT GLCD Adapter created with CoIDE but may be built wiht simple Makefile. Also you may use VSCode as IDE.

Read and change **inc/defines.h** to set proper parameters.

## Main parameters
* `HW_VER_1`                - Hardware based on "BluePill" board, with encoder. Used for LCD with 8 bit data bus.
* `HW_VER_2`                - HW VER.2 has different controller pinout !!!!! Used for LCD with 16 bit data bus.
* `HW_VER_3`                - Panel with resistive touchscreen without encoder. Used for LCD with 8 bit data bus.
* `TEST_TOUCH`              - for test touchscreen and get calibrate constants.
* `INVERT_ENCODER_DIR`      - change direction for encoder
* `SET_ORIENT_RIGHT (LEFT)` - set LCD orientation by software
* `ILI9325`, `ILI9327`,
* `ILI9341`, `ST7789`       - LCD chip. Select only one of them
* `LCD_BRIGHTNES`           - initial brightnes for LCD
* `WITHOUT_HEAT_ICO`        - if you want see "FAN %" text insted heat icon on 320x240 screen
* `FONT`                    - font for text. Courier_New_Bold or LiberationMono
* `BUZ_FREQ_MP`             - buzzer input frequency multiplier. Change to get more suitable tones
* `LOGO_FREQ`               - buzzer frequency for start logo
* `I2C_Addr`                - I2C addres, for Marlin, must be the same as param in Conditionals_LCD.h