#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "defines.h"
#include "init.h"
#include "systick.h"
#include "system_stm32f10x.h"

static void Init_GPIO(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	// Enabling clock for GPIO A,B
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

	// Init LCD control bus and LCD Data bus
	GPIO_InitStructure.GPIO_Pin    = LCD_RST | LCD_WR | LCD_RS | LCD_CS;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(LCD_CTRL_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin    = LCD_DATA_MASK;
	GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);

	// Init Encoder lines
	GPIO_InitStructure.GPIO_Pin    = ENC_A | ENC_B | ENC_BUT;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU;
	GPIO_Init(ENC_PORT, &GPIO_InitStructure);

	// Init Button
	GPIO_InitStructure.GPIO_Pin    = BUTTON_PIN;
	GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);

  	// PWM signals init
    GPIO_InitStructure.GPIO_Pin    = CONTRAST_PIN | BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
    GPIO_Init(CONTRAST_PORT, &GPIO_InitStructure);

    // Configure pins for SPI2
    GPIO_InitStructure.GPIO_Pin = SPI_MOSI | SPI_MISO | SPI_SCK | SPI_CS;
    GPIO_Init(SPI_PORT, &GPIO_InitStructure);
}
/***********************************************************
 Timer1 for encoder
***********************************************************/
static void Timer1_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->CCMR1 |= TIM_CCMR1_IC1F | TIM_CCMR1_IC2F;
	TIM1->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
	TIM1->CCER = TIM_CCER_CC1P | TIM_CCER_CC2P;
	TIM1->SMCR = TIM_SMCR_SMS_0;
	TIM1->ARR = 0xffff;
	TIM1->CNT = 0;
	TIM1->CR1 |=  TIM_CR1_CEN;
}
/***********************************************************
 Timer3 for PWM (BUZZER, CONTRAST)
***********************************************************/
static void Timer3_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	uint16_t PrescalerValue = 0;

	// TIM3 clock enable
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  	if (CONTRAST_PIN != GPIO_Pin_1)
  		AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_1;

	// Compute the prescaler value
	PrescalerValue = (uint16_t) (SystemCoreClock / 250000);

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
#ifdef	BUZZER_PIN4
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif
#ifdef	BUZZER_PIN5
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif

    // PWM1 Mode configuration for Contrast
	TIM_OCInitStructure.TIM_Pulse = INIT_CONTRAST;
#ifdef	CONTRAST_PIN0
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif
#ifdef	CONTRAST_PIN1	//Channel4
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif
#ifdef	CONTRAST_PIN4	//Channel1
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif
#ifdef	CONTRAST_PIN5	//Channel2
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	TIM_Cmd(TIM3, ENABLE);
}
/***********************************************************
***********************************************************/
void spi_init()
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
void Global_Init(void)
{
	SystemInit();
	delay_init();
	Init_GPIO();
	Timer1_init();
	Timer3_init();
	spi_init();
}
