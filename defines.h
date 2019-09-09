#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <stdint.h>
#include "stm32f10x_gpio.h"

enum protocols {Smoothie = 0, Marlin1 = 1, Marlin2 = 2};

//select only used LCD
#define ILI9325
//#define ILI9327
//#define ILI9341

//display orientations
#define	LANDSCAPE_L	//LCD chip placed left

//prefered font
//#define FONT	Courier_New_Bold_16x24
#define FONT	LiberationMono_16x24


#if defined(ILI9325) || defined(ILI9341)
#define	LCDMIN	240
#define	LCDMAX	320
#define LCD320x240
#endif
#ifdef ILI9327
#define	LCDMIN	240
#define	LCDMAX	400
#define LCD400x240
#endif

#define	LCDXMAX	LCDMAX
#define	LCDYMAX	LCDMIN

//monospace font 16õ24
#define	CHAR_WIDTH	16
#define	CHAR_HEIGTH	24
#define	CHAR_BYTES	(CHAR_WIDTH * CHAR_HEIGTH / 8)

#define	CHARS_PER_LINE	LCDXMAX/CHAR_WIDTH
#define	TEXT_LINES		LCDYMAX/CHAR_HEIGTH

//LCD interface
#define LCD_DATA_PORT	GPIOA
#define	LCD_DATA_MASK	0x00ff
#define LCD_CTRL_PORT	GPIOB
#define	LCD_RST			GPIO_Pin_6
#define	LCD_WR			GPIO_Pin_7
#define	LCD_RS			GPIO_Pin_8
#define	LCD_CS			GPIO_Pin_9
//SPI
#define	SPI				SPI2
#define SPI_PORT		GPIOB
#define	SPI_CS			GPIO_Pin_12
#define	SPI_SCK			GPIO_Pin_13
#define	SPI_MISO		GPIO_Pin_14
#define	SPI_MOSI		GPIO_Pin_15
#define SPI_RCC			RCC_APB1Periph_SPI2
#define SPI_GPIO_RCC	RCC_APB2Periph_GPIOB
#define	SPI_IRQ			SPI2_IRQn
//I2C
#define I2C				I2C2
#define I2C_PORT		GPIOB
#define	I2C_SCL			GPIO_Pin_10
#define	I2C_SDA			GPIO_Pin_11
#define I2C_RCC			RCC_APB1Periph_I2C2
#define	I2C_IRQ			I2C2_EV_IRQn
#define	I2C_ERR_IRQ		I2C2_ER_IRQn
#define I2C_Addr		0x20 << 1		//as PCA8574 and PCF8575
//encoder
#define	ENC_PORT        GPIOA
#define	ENC_A           GPIO_Pin_9
#define	ENC_B           GPIO_Pin_8
#define	ENC_BUT         GPIO_Pin_10
//buttons
#define	BTN_PORTA	    GPIOA
#define	BUTTON_PIN1	    GPIO_Pin_11
#define	BUTTON_PIN2	    GPIO_Pin_12
#define	BUTTON_PIN3	    GPIO_Pin_15
#define	BTN_PORTB	    GPIOB
#define	BUTTON_PIN4	    GPIO_Pin_3
#define	BUTTON_PIN5	    GPIO_Pin_4

#define	CONTRAST_PORT	GPIOB
#define	BUZZER_PIN1		//BUZZER_PIN0 or BUZZER_PIN1
#define	CONTRAST_PIN0	//CONTRAST_PIN1 or CONTRAST_PIN0
#define	INIT_CONTRAST	200

#ifdef	BUZZER_PIN0
 #define	BUZZER_PIN		GPIO_Pin_0
 #define	BUZZER_CCR		CCR3
#endif
#ifdef	BUZZER_PIN1
 #define	BUZZER_PIN		GPIO_Pin_1
 #define	BUZZER_CCR		CCR4
#endif

#ifdef	CONTRAST_PIN0
 #define	CONTRAST_PIN	GPIO_Pin_0
 #define	CONTRAST_CCR	CCR3
#endif
#ifdef	CONTRAST_PIN1
 #define	CONTRAST_PIN	GPIO_Pin_1
 #define	CONTRAST_CCR	CCR4
#endif

//buttons bits for Smoothie
#define BUTTON_SELECT 0x01
#define BUTTON_RIGHT  0x02
#define BUTTON_DOWN   0x04
#define BUTTON_UP     0x08
#define BUTTON_LEFT   0x10
#define BUTTON_PAUSE  0x20
#define BUTTON_AUX1   0x40
#define BUTTON_AUX2   0x80
//buttons bits for Marlin as for NEWPANEL
#define EN_A  0x01	// enc_a - reserved
#define EN_B  0x02	// enc_b - reserved
#define EN_C  0x04	// enc_but
#define EN_D  0x08	// back ??
#define KILL  0x10

//led bits
#define LED_HOTEND	0x01
#define LED_BED		0x02
#define LED_FAN		0x04
#define LED_HOT		0x08
#define	LED_MASK	LED_HOT | LED_FAN | LED_BED | LED_HOTEND

//icon bits
#define	PIC_LOGO	0x01
#define	PIC_HE1		0x02
#define	PIC_HE2		0x04
#define	PIC_HE3		0x08
#define	PIC_BED		0x10
#define	PIC_FAN		0x20
#define PIC_HOT     0x40
#define PIC_MASK	PIC_HOT | PIC_FAN | PIC_BED | PIC_HE3 | PIC_HE2 | PIC_HE1 | PIC_LOGO

#define	TEST_PORT	GPIOB
#define	TEST_PIN	GPIO_Pin_5
#endif
