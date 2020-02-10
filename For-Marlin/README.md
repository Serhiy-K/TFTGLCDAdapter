For use TFTGLCD Adapter with Marlin you need:
1. ADD/replace files in Marlin (for Marlin-1) or Marlin-bugfix-2.0.x\Marlin directory with new from this directiry excluding Configuration.h - it for example only.
2. Comment line with current selected panel in your **Configuration.h** file and place new line with "#define TFTGLCD_ADAPTER" insted.
3. You need check section with "#elif ENABLED(TFTGLCD_ADAPTER)" in **Conditionals_LCD.h** (for Marlin-1) or **src\inc\Conditionals_LCD.h** (for Marlin-2) for panel's right parameters.
4. If you are planning connect TFTGLCD Adapter by SPI bus you need add some new lines to file `"pins_YOURS_BOARD_NAME.h"`. Simplest way to do this - change section with `"#if HAS_SPI_LCD"`. For examle, old section:

`#if HAS_SPI_LCD
  #define BEEPER_PIN       P1_30   // (37) not 5V tolerant
  #define BTN_ENC          P0_28   // (58) open-drain

  #if ENABLED(CR10_STOCKDISPLAY)
    #define LCD_PINS_RS    P1_22

    #define BTN_EN1        P1_18
    #define BTN_EN2        P1_20
.......
.......
#endif // HAS_SPI_LCD`


new section:

`#if ENABLED(TFTGLCD_ADAPTER)
      #define BEEPER_PIN      -1    //disable board beeper
      #define BTN_ENC         -1    //disable board encoder
      #define BTN_EN1         -1
      #define BTN_EN2         -1
      #define DOGLCD_CS       P3_26 //on EXPn connector with SPI bus
#else
#if HAS_SPI_LCD
  #define BEEPER_PIN       P1_30   // (37) not 5V tolerant
  #define BTN_ENC          P0_28   // (58) open-drain

  #if ENABLED(CR10_STOCKDISPLAY)
    #define LCD_PINS_RS    P1_22

    #define BTN_EN1        P1_18
    #define BTN_EN2        P1_20
.......
.......
#endif // HAS_SPI_LCD

#endif	//TFTGLCD_ADAPTER`

You may use disabled beeper and encoder pins for other tasks.