For use TFTGLCD Panel with Marlin-1 you need:
1. ADD/replace files into your Marlin directory with new files from this directiry excluding Configuration.h - it for example only.
2. Comment line with current selected panel in your Configuration.h file and place new line with "#define TFTGLCD_ADAPTER" insted.
3. You need check section with "#elif ENABLED(TFTGLCD_ADAPTER)" in Conditionals_LCD.h for panel's parameters.

Common for Marlin-1 and Marlin-2:

If you are planning connect TFTGLCD Panel by SPI bus you need add some new lines to file pins_YOURS_BOARD_NAME.h.
Simplest way to do this - change section with "#if HAS_SPI_LCD".

For examle, old section:

#if HAS_SPI_LCD
  #if ENABLED(CR10_STOCKDISPLAY)
    #define LCD_PINS_RS    P1_22
    #define BEEPER_PIN     P1_30   // (37) not 5V tolerant
    #define BTN_ENC        P0_28   // (58) open-drain
    #define BTN_EN1        P1_18
    #define BTN_EN2        P1_20
.......
.......
#endif // HAS_SPI_LCD

new section:

#if HAS_SPI_LCD
  #if ENABLED(TFTGLCD_ADAPTER)
    #define BEEPER_PIN     -1     //disable board beeper
    #define BTN_ENC        -1     //disable board encoder
    #define BTN_EN1        -1
    #define BTN_EN2        -1
    #define DOGLCD_CS      P3_26  //on EXPn connector with SPI bus
  #elif ENABLED(CR10_STOCKDISPLAY)
    #define LCD_PINS_RS    P1_22
    #define BEEPER_PIN     P1_30   // (37) not 5V tolerant
    #define BTN_ENC        P0_28   // (58) open-drain
    #define BTN_EN1        P1_18
    #define BTN_EN2        P1_20
.......
.......
#endif // HAS_SPI_LCD

For Marlin-2 text "TFTGLCD_ADAPTER" above must be replaced with "IS_TFTGLCD_PANEL" and text "DOGLCD_CS" with "TFTGLCD_CS".
Also, Marlin's source code already has TFTGLCD PANEL support for some boards.
In new section main description is "DOGLCD_CS" ("TFTGLCD_CS") for SPI connection.

You may use disabled beeper and encoder pins for other tasks.

This time TFTGLCD Panel tested on Marlin-1 with Mega2560 MCU and Marlin-2 with Mega2560, LPC1768, STM32F103CB and STM32F411CU MCUs.
TFTGLCD Panel connected to boards with mentioned MCUs by SPI and I2C buses.
