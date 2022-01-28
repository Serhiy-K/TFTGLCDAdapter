#ifndef __SYS_H
#define __SYS_H	

#include "stm32f10x.h"

//SPI
#define SPI             SPI2
#define SPI_PORT        GPIOB
#define SPI_CS          GPIO_Pin_12
#define SPI_SCK         GPIO_Pin_13
#define SPI_MISO        GPIO_Pin_14
#define SPI_MOSI        GPIO_Pin_15
#define SPI_RCC         RCC_APB1Periph_SPI2
#define SPI_GPIO_RCC    RCC_APB2Periph_GPIOB

#define BOOT_PORT       GPIOC
#define BOOT_PIN        GPIO_Pin_15
#define BOOT_GPIO_RCC   RCC_APB2Periph_GPIOC

#endif  //__SYS_H
