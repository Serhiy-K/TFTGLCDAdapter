#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <stdint.h>
#include "stm32f10x_gpio.h"

#define ONE_HOTEND

//display orientations
#ifdef	PORTRET
#define	LCDXMAX	240
#define	LCDYMAX	320
#define	PORTRET_B
//#define	PORTRET_T
#else
#define	LCDXMAX	320
#define	LCDYMAX	240
#define	LANDSCAPE_L
//#define	LANDSCAPE_R
#endif
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
//encoder
#define	ENC_PORT	GPIOA
#define	ENC_A		GPIO_Pin_9
#define	ENC_B		GPIO_Pin_8
#define	ENC_BUT		GPIO_Pin_10
//single button. You may define pins up to 8 buttons
#define	BUTTON_PORT	GPIOB
#define	BUTTON_PIN	GPIO_Pin_11

#define	CONTRAST_PORT	GPIOB
#define	BUZZER_PIN1		//BUZZER_PIN0 or BUZZER_PIN1 or BUZZER_PIN4 or BUZZER_PIN5
#define	CONTRAST_PIN0	//CONTRAST_PIN0 or CONTRAST_PIN1 or CONTRAST_PIN4 or CONTRAST_PIN5

#ifdef	BUZZER_PIN0
 #define	BUZZER_PIN		GPIO_Pin_0
 #define	BUZZER_CCR		CCR3
#endif
#ifdef	BUZZER_PIN1
 #define	BUZZER_PIN		GPIO_Pin_1
 #define	BUZZER_CCR		CCR4
#endif
#ifdef	BUZZER_PIN4
 #define	BUZZER_PIN		GPIO_Pin_4
 #define	BUZZER_CCR		CCR1
#endif
#ifdef	BUZZER_PIN5
 #define	BUZZER_PIN		GPIO_Pin_5
 #define	BUZZER_CCR		CCR2
#endif

#ifdef	CONTRAST_PIN0
 #define	CONTRAST_PIN	GPIO_Pin_0
 #define	CONTRAST_CCR	CCR3
#endif
#ifdef	CONTRAST_PIN1
 #define	CONTRAST_PIN	GPIO_Pin_1
 #define	CONTRAST_CCR	CCR4
#endif
#ifdef	CONTRAST_PIN4
 #define	CONTRAST_PIN	GPIO_Pin_4
 #define	CONTRAST_CCR	CCR1
#endif
#ifdef	CONTRAST_PIN5
 #define	CONTRAST_PIN	GPIO_Pin_5
 #define	CONTRAST_CCR	CCR2
#endif
#define	INIT_CONTRAST	200

//buttons bits
#define BUTTON_SELECT 0x01
#define BUTTON_RIGHT  0x02
#define BUTTON_DOWN   0x04
#define BUTTON_UP     0x08
#define BUTTON_LEFT   0x10
#define BUTTON_PAUSE  0x20
#define BUTTON_AUX1   0x40
#define BUTTON_AUX2   0x80

//led bits
#define LED_HOTEND	0x01
#define LED_BED		0x02
#define LED_FAN		0x04
#define LED_HOT		0x08
#define	LED_MASK	LED_HOT | LED_FAN | LED_BED | LED_HOTEND

//icon bits
#define	PIC_LOGO	0x01
#define	PIC_HOT1	0x02
#define	PIC_HOT2	0x04
#define	PIC_HOT3	0x08
#define	PIC_BED		0x10
#define	PIC_FAN		0x20
#define PIC_MASK	PIC_FAN | PIC_BED | PIC_HOT3 | PIC_HOT2 | PIC_HOT1 | PIC_LOGO

#endif
