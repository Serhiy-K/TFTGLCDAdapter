#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <stdint.h>
#include "stm32f10x_gpio.h"

enum protocols {Smoothie = 0, Marlin = 1};

//============================================================================================
//                                Main parameters
//============================================================================================
//For LCD and controller with 8 bit data bus and encoder
//#define HW_VER_1
//For LCD and controller with 16 bit data bus and encoder
#define HW_VER_2
//For LCD and controller with 8 bit data bus and touchscreen
//#define HW_VER_3
//#define CALIBR_DEBUG_INFO   //outputs old and new ADC values

//#define HW_VER_2_SWD_DEBUG

#define INVERT_ENCODER_DIR

// WARNING !!!!!
//To hardware select proper display orientations (side of the LCD connecting cable) you need
//pull LCD_ORIENT_PIN by resistor with about 10k to GND for right side or to +3.3V for left
//or use one of SET_ORIENT_ parameter for software select orientation.
#define SET_ORIENT_RIGHT
//#define SET_ORIENT_LEFT

//select only one LCD type
#define ILI9325   //ILI9325==ILI9328
//#define ILI9341
//#define ST7789
//#define ILI9327
//#define R61509V
//#define HX8347

#define LCD_BRIGHTNES   250

#define WITHOUT_HEAT_ICO    //if you want see "FAN %" text insted heat icon on 320x240 screen

//you may select only one variant for MCU with 64k ROM or both for universality for MCU with 128k ROM
#define USE_3DPRINTER       //for Smoothieware and Marlin
#define USE_LASER           //only for Marlin

//prefered monospace font
//#define FONT          Courier_New_Bold_16x24
#define FONT            LiberationMono_16x24

//buzzer frequency
#define BUZ_FREQ_MP     1.8     //frequency multiplier. Change to get more suitable tones
#define LOGO_FREQ1      800 / BUZ_FREQ_MP
#define LOGO_FREQ2      900 / BUZ_FREQ_MP
#define LOGO_FREQ3      1000 / BUZ_FREQ_MP

#define CHAR_WIDTH      16  //depend on FONT resolution
#define CHAR_HEIGTH     24

#define I2C_Panel_Addr  0x33    //must be 0x33 for STM32 main boards and must be equal to LCD_I2C_ADDRESS parameter in Conditionals_LCD.h
#define I2C_Addr        I2C_Panel_Addr << 1

//============================================================================================

#if defined(ILI9325) || defined(ILI9341) || defined(ST7789) || defined(HX8347)
#define LCDMIN          240
#define LCDMAX          320
#define LCD320x240
#endif
#if defined(ILI9327) || defined(R61509V)
#define LCDMIN          240
#define LCDMAX          384	//for even number of symols in line
#define LCD400x240
#endif

#define LCDXMAX         LCDMAX
#define LCDYMAX         LCDMIN

#define CHAR_BYTES      CHAR_WIDTH * CHAR_HEIGTH / 8
#define CHARS_PER_LINE  LCDXMAX / CHAR_WIDTH
#define TEXT_LINES      LCDYMAX / CHAR_HEIGTH
#define MIDDLE_Y        (TEXT_LINES - 1) / 2

#if defined(HW_VER_1)

#define TEST_PORT       GPIOC
#define TEST_PIN        GPIO_Pin_13

//LCD interface
#define LCD_DATA_PORT   GPIOA
#define LCD_DATA_MASK   0x00ff
#define LCD_ORIENT_PORT GPIOB
#define LCD_ORIENT_PIN  GPIO_Pin_5
#define LCD_CTRL_PORT   GPIOB
#define LCD_RD          GPIO_Pin_5
#define LCD_RST         GPIO_Pin_6
#define LCD_WR          GPIO_Pin_7
#define LCD_RS          GPIO_Pin_8
#define LCD_CS          GPIO_Pin_9
//SPI
#define SPI             SPI2
#define SPI_PORT        GPIOB
#define SPI_CS          GPIO_Pin_12
#define SPI_SCK         GPIO_Pin_13
#define SPI_MISO        GPIO_Pin_14
#define SPI_MOSI        GPIO_Pin_15
#define SPI_RCC         RCC_APB1Periph_SPI2
#define SPI_GPIO_RCC    RCC_APB2Periph_GPIOB
#define SPI_IRQ         SPI2_IRQn
#define SPI_IRQHandler  SPI2_IRQHandler
//I2C
#define I2C             I2C2
#define I2C_PORT        GPIOB
#define I2C_SCL         GPIO_Pin_10
#define I2C_SDA         GPIO_Pin_11
#define I2C_RCC         RCC_APB1Periph_I2C2
#define I2C_IRQ         I2C2_EV_IRQn
#define I2C_ERR_IRQ     I2C2_ER_IRQn
//encoder
#define ENC_PORT        GPIOA
#define ENC_A           GPIO_Pin_9
#define ENC_B           GPIO_Pin_8
#define ENC_IRQn        EXTI9_5_IRQn
#define ENC_IRQHandler  EXTI9_5_IRQHandler
//buttons
#define BTN_PORTA       GPIOA
#define ENC_BUT         GPIO_Pin_10
#define BUTTON_PIN1     GPIO_Pin_11
#define BUTTON_PIN2     GPIO_Pin_12
#define BUTTON_PIN3     GPIO_Pin_15
#define BTN_PORT2       GPIOB
#define BUTTON_PIN4     GPIO_Pin_3
#define BUTTON_PIN5     GPIO_Pin_4
#define BUTTONS_A_MSK   (BUTTON_PIN3 | BUTTON_PIN2 | BUTTON_PIN1 | ENC_BUT)
//PWM outputs
#define PWM_PORT        GPIOB
#define BRIGHTNES_PIN   GPIO_Pin_0
#define BRIGHTNES_CCR   CCR3
#define BUZZER_PIN      GPIO_Pin_1
#define BUZZER_CCR      CCR4
//Timers
#define Timer_D         TIM2    //Duration
#define Dur_RCC_ENR     RCC_APB1ENR_TIM2EN
#define Duration_IRQ    TIM2_IRQn
#define Dur_IRQHandler  TIM2_IRQHandler
#define Timer_P         TIM3    //PWM
#define PWM_RCC_ENR     RCC_APB1Periph_TIM3
#define Timer_Del       TIM4    //delay
#define Delay_RCC_ENR   RCC_APB1ENR_TIM4EN
#define Delay_IRQ       TIM4_IRQn
#define Del_IRQHandler  TIM4_IRQHandler

#elif defined(HW_VER_2)

//LCD interface
#define LCD_DATA_PORT   GPIOA
#ifdef HW_VER_2_SWD_DEBUG
 #define LCD_DATA_MASK  0x9fff  //exclude 2 pins for SWD for debug
#else
 #define LCD_DATA_MASK  0xffff
#endif
#define LCD_ORIENT_PORT GPIOB
#define LCD_ORIENT_PIN  GPIO_Pin_4

#define LCD_CTRL_PORT   GPIOB
#define LCD_RST         GPIO_Pin_3
#define LCD_RD          GPIO_Pin_4
#define LCD_WR          GPIO_Pin_5
#define LCD_RS          GPIO_Pin_6
#define LCD_CS          GPIO_Pin_7
//SPI
#define SPI             SPI2
#define SPI_PORT        GPIOB
#define SPI_CS          GPIO_Pin_12
#define SPI_SCK         GPIO_Pin_13
#define SPI_MISO        GPIO_Pin_14
#define SPI_MOSI        GPIO_Pin_15
#define SPI_RCC         RCC_APB2Periph_SPI2
#define SPI_GPIO_RCC    RCC_APB2Periph_GPIOB
#define SPI_IRQ         SPI2_IRQn
#define SPI_IRQHandler  SPI2_IRQHandler
//I2C
#define I2C             I2C2
#define I2C_PORT        GPIOB
#define I2C_SCL         GPIO_Pin_10
#define I2C_SDA         GPIO_Pin_11
#define I2C_RCC         RCC_APB1Periph_I2C2
#define I2C_IRQ         I2C2_EV_IRQn
#define I2C_ERR_IRQ     I2C2_ER_IRQn
//encoder
#define ENC_PORT        GPIOB
#define ENC_A           GPIO_Pin_0
#define ENC_B           GPIO_Pin_1
#define ENC_IRQn        EXTI0_IRQn
#define ENC_IRQHandler  EXTI0_IRQHandler
//buttons
#define BTN_PORTA       GPIOB
#define ENC_BUT         GPIO_Pin_2
#define BTN_PORT2       GPIOC
#define BUTTON_PIN1     GPIO_Pin_13
#define BUTTON_PIN2     GPIO_Pin_14
#define BUTTON_PIN3     GPIO_Pin_15
#define BUTTON_PIN4     (GPIO_Pin_13 | GPIO_Pin_14)
#define BUTTON_PIN5     (GPIO_Pin_14 | GPIO_Pin_15)
#define BUTTONS2_MSK    (GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13)
//PWM outputs
#define PWM_PORT        GPIOB
#define BRIGHTNES_PIN   GPIO_Pin_8
#define BRIGHTNES_CCR   CCR3
#define BUZZER_PIN      GPIO_Pin_9
#define BUZZER_CCR      CCR4
//Timers
#define Timer_D         TIM2    //Duration
#define Dur_RCC_ENR     RCC_APB1ENR_TIM2EN
#define Duration_IRQ    TIM2_IRQn
#define Dur_IRQHandler  TIM2_IRQHandler
#define Timer_P         TIM4    //PWM
#define PWM_RCC_ENR     RCC_APB1Periph_TIM4
#define Timer_Del       TIM3    //delay
#define Delay_RCC_ENR   RCC_APB1ENR_TIM3EN
#define Delay_IRQ       TIM3_IRQn
#define Del_IRQHandler  TIM3_IRQHandler

#elif defined(HW_VER_3)

#define TEST_PORT       GPIOC
#define TEST_PIN        GPIO_Pin_13

//LCD interface
#define LCD_DATA_PORT   GPIOB
#define LCD_DATA_MASK   0x00ff
#define LCD_ORIENT_PORT GPIOB
#define LCD_ORIENT_PIN  GPIO_Pin_9
#define LCD_CTRL_PORT   GPIOB
#define LCD_WR          GPIO_Pin_8
#define LCD_RD          GPIO_Pin_9
#define LCD_CTRL_PORT2  GPIOA
#define LCD_RS          GPIO_Pin_10
#define LCD_CS          GPIO_Pin_11
#define LCD_RST         GPIO_Pin_12
//SPI
#define SPI             SPI2
#define SPI_PORT        GPIOB
#define SPI_CS          GPIO_Pin_12
#define SPI_SCK         GPIO_Pin_13
#define SPI_MISO        GPIO_Pin_14
#define SPI_MOSI        GPIO_Pin_15
#define SPI_RCC         RCC_APB1Periph_SPI2
#define SPI_GPIO_RCC    RCC_APB2Periph_GPIOB
#define SPI_IRQ         SPI2_IRQn
#define SPI_IRQHandler  SPI2_IRQHandler
//I2C
#define I2C             I2C2
#define I2C_PORT        GPIOB
#define I2C_SCL         GPIO_Pin_10
#define I2C_SDA         GPIO_Pin_11
#define I2C_RCC         RCC_APB1Periph_I2C2
#define I2C_IRQ         I2C2_EV_IRQn
#define I2C_ERR_IRQ     I2C2_ER_IRQn
//touchscreen
#define TS_PORT         GPIOA
#define TS_XR           GPIO_Pin_0  //4 pins for LEFT ORIENTATION screen !!!
#define TS_YU           GPIO_Pin_1
#define TS_XL           GPIO_Pin_2
#define TS_YD           GPIO_Pin_3
#define ADC_RCC         RCC_APB2Periph_ADC1
//buttons
#define BTN_PORTA       GPIOA
#define BUTTON_PIN1     GPIO_Pin_4
#define BUTTON_PIN2     GPIO_Pin_5
#define BUTTON_PIN3     GPIO_Pin_8
#define BUTTON_PIN4     GPIO_Pin_9
#define BUTTONS_A_MSK   (BUTTON_PIN4 | BUTTON_PIN3 | BUTTON_PIN2 | BUTTON_PIN1)
//PWM outputs
#define PWM_PORT        GPIOA
#define BRIGHTNES_PIN   GPIO_Pin_7
#define BRIGHTNES_CCR   CCR2
#define BUZZER_PIN      GPIO_Pin_6
#define BUZZER_CCR      CCR1
//Timers
#define Timer_D         TIM2    //Duration
#define Dur_RCC_ENR     RCC_APB1ENR_TIM2EN
#define Duration_IRQ    TIM2_IRQn
#define Dur_IRQHandler  TIM2_IRQHandler
#define Timer_P         TIM3    //PWM
#define PWM_RCC_ENR     RCC_APB1Periph_TIM3
#define Timer_Del       TIM4    //delay
#define Delay_RCC_ENR   RCC_APB1ENR_TIM4EN
#define Delay_IRQ       TIM4_IRQn
#define Del_IRQHandler  TIM4_IRQHandler

#endif

#define Timer_Btn       TIM1
#define Btn_RCC_ENR     RCC_APB2ENR_TIM1EN
#define Btn_IRQ         TIM1_UP_IRQn
#define Btn_IRQHandler  TIM1_UP_IRQHandler

//button bits for Smoothie
#define BUTTON_SELECT   0x01    //encoder button
#define BUTTON_RIGHT    0x02
#define BUTTON_DOWN     0x04
#define BUTTON_UP       0x08
#define BUTTON_LEFT     0x10    //don't works in Smoothie?
#define BUTTON_PAUSE    0x20    //don't works in Smoothie?
#define BUTTON_AUX1     0x40
#define BUTTON_AUX2     0x80
//button bits for Marlin as for NEWPANEL
#define EN_A            0x01    // enc_a - reserved
#define EN_B            0x02    // enc_b - reserved
#define EN_C            0x04    // enc_but
#define EN_D            0x08    // back
#define KILL            0x10

//led bits
#define LED_HOTEND      0x01
#define LED_BED         0x02
#define LED_FAN         0x04
#define LED_HOT         0x08
#define LED_MASK        LED_HOT | LED_FAN | LED_BED | LED_HOTEND

//icon bits
#define PIC_LOGO	    0x01
#define PIC_HE1         0x02
#define PIC_HE2         0x04
#define PIC_HE3         0x08
#define PIC_BED         0x10
#define PIC_FAN         0x20
#define PIC_HOT         0x40
#define PIC_MASK        PIC_HOT | PIC_FAN | PIC_BED | PIC_HE3 | PIC_HE2 | PIC_HE1 | PIC_LOGO

#endif  //_DEFINES_H_
