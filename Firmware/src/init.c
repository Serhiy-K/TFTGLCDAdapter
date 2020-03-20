#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "defines.h"
#include "init.h"
#include "systick.h"
#include "system_stm32f10x.h"
#include "LCD.h"

static void GPIO_init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	// Enabling clock for GPIO A,B,C
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;

	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

	// Init LCD control bus and LCD Data bus
	LCD_CTRL_PORT->BSRR = LCD_RD | LCD_RST | LCD_WR | LCD_RS | LCD_CS;
	GPIO_InitStructure.GPIO_Pin    = LCD_RD | LCD_RST | LCD_WR | LCD_RS | LCD_CS;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(LCD_CTRL_PORT, &GPIO_InitStructure);
#ifdef LCD_16BIT_BUS
	GPIO_InitStructure.GPIO_Pin    = LCD_H_WR;
	GPIO_Init(LCD_H_PORT, &GPIO_InitStructure);
#endif
	GPIO_InitStructure.GPIO_Pin    = LCD_DATA_MASK;
	GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);

	CS_LCD_set;	WR_LCD_set;	RS_LCD_set;	RES_LCD_set;

	// Init Encoder lines
	GPIO_InitStructure.GPIO_Pin    = ENC_A | ENC_B;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU;
	GPIO_Init(ENC_PORT, &GPIO_InitStructure);

	// Init Button
	GPIO_InitStructure.GPIO_Pin    = BUTTONS_A_MSK;
	GPIO_Init(BTN_PORTA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin    = BUTTON_PIN5 | BUTTON_PIN4;
	GPIO_Init(BTN_PORTB, &GPIO_InitStructure);

  	// PWM signals init
    GPIO_InitStructure.GPIO_Pin    = BRIGHTNES_PIN | BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
    GPIO_Init(BRIGHTNES_PORT, &GPIO_InitStructure);

    // Configure pins for SPI2
    GPIO_InitStructure.GPIO_Pin = SPI_MOSI | SPI_MISO | SPI_SCK | SPI_CS;
    GPIO_Init(SPI_PORT, &GPIO_InitStructure);

    // Configure pins for I2C2
    GPIO_InitStructure.GPIO_Pin		= I2C_SCL | I2C_SDA;
    GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_OD;
    GPIO_Init(I2C_PORT, &GPIO_InitStructure);

    //for test output
	GPIO_InitStructure.GPIO_Pin		= TEST_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_Init(TEST_PORT, &GPIO_InitStructure);
}
/***********************************************************
 Timer1 for encoder
***********************************************************/
static void Timer1_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->CCMR1 |= TIM_CCMR1_IC1F | TIM_CCMR1_IC2F;
	TIM1->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
#ifdef	INVERT_ENCODER_DIR
	TIM1->CCER = TIM_CCER_CC1P;
#else
	TIM1->CCER = TIM_CCER_CC1P | TIM_CCER_CC2P;
#endif
	TIM1->SMCR = TIM_SMCR_SMS_0;
	TIM1->ARR = 0xffff;
	TIM1->CNT = 0;
	TIM1->CR1 |= TIM_CR1_CEN;	//one puls mode
}
/***********************************************************
 Timer2 for buzzer duration
***********************************************************/
static void Timer2_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->CR1 = TIM_CR1_URS; // up counter, only overflow interrupt
	TIM2->PSC = 72;	// FclkT2 = 1MHz
	TIM2->CNT = 0;
	TIM2->ARR = 1000;	// 1ms period
	TIM2->DIER = TIM_DIER_UIE;
	TIM2->EGR = TIM_EGR_UG;
	NVIC_EnableIRQ(TIM2_IRQn);
}
/***********************************************************
 Timer3 for PWM (BUZZER, BRIGHTNES)
***********************************************************/
static void Timer3_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	uint16_t PrescalerValue = 0;

	// TIM3 clock enable
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  	if (BRIGHTNES_PIN != GPIO_Pin_1)
  		AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_1;

	// Compute the prescaler value
	PrescalerValue = (uint16_t) (SystemCoreClock / 256000);	//frequency = 1kHz for Period = 255

	// Time base configuration
	TIM_TimeBaseStructure.TIM_Period = 255;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // PWM1 Mode configuration for Buzzer
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
#ifdef	BUZZER_PIN0
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif
#ifdef	BUZZER_PIN1
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif

    // PWM1 Mode configuration for BRIGHTNES
	TIM_OCInitStructure.TIM_Pulse = INIT_BRIGHTNES;
#ifdef	BRIGHTNES_PIN0
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif
#ifdef	BRIGHTNES_PIN1	//Channel4
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	TIM_Cmd(TIM3, ENABLE);
}
/***********************************************************
***********************************************************/
static void SPI_init()
{
    SPI_InitTypeDef		SPI_InitStructure;

    // Configure the clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    SPI_Cmd(SPI, DISABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    //SPI_CPOL and SPI_CPHA for Smoothie protocol
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI, &SPI_InitStructure);
    SPI_Cmd(SPI, ENABLE);

    SPI_I2S_ITConfig(SPI, SPI_I2S_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(SPI_IRQ);
}
/***********************************************************
***********************************************************/
static void I2C_init()
{
    I2C_InitTypeDef		I2C_InitStructure;

    // Configure the clocks
    RCC_APB1PeriphClockCmd(I2C_RCC, ENABLE);

    I2C_Cmd(I2C, DISABLE);
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = I2C_Addr;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C, &I2C_InitStructure);
    I2C_Cmd(I2C, ENABLE);

    I2C_ITConfig(I2C, I2C_IT_EVT, ENABLE);
    I2C_ITConfig(I2C, I2C_IT_BUF, ENABLE);
    I2C_ITConfig(I2C, I2C_IT_ERR, ENABLE);

    NVIC_SetPriority (I2C_IRQ, 0);
    NVIC_EnableIRQ   (I2C_IRQ);
    NVIC_SetPriority (I2C_ERR_IRQ, 0);
    NVIC_EnableIRQ   (I2C_ERR_IRQ);
}
/***********************************************************
***********************************************************/
void Global_Init(void)
{
	SystemInit();
	delay_init();
	GPIO_init();
	Timer1_init();
	Timer2_init();
	Timer3_init();
	SPI_init();
	I2C_init();
	LCD_Init();
}
