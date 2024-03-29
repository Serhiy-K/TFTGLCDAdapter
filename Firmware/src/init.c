#include "defines.h"
#include "stm32f10x.h"
#ifdef HW_VER_3
#include "stm32f10x_adc.h"
#include "settings.h"
#endif
#include "stm32f10x_exti.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "init.h"
#include "systick.h"
#include "system_stm32f10x.h"
#include "LCD.h"

extern uint8_t orientation;

static void GPIO_init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	// Enabling clock for GPIO A,B,C, AFIO
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;

	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
#if !defined(SET_ORIENT_RIGHT) && !defined(SET_ORIENT_LEFT)
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin	= LCD_ORIENT_PIN;
	GPIO_Init(LCD_ORIENT_PORT, &GPIO_InitStructure);
	orientation = LCD_ORIENT_PORT->IDR & LCD_ORIENT_PIN;	//get LCD orientation
#else
#ifdef SET_ORIENT_RIGHT
	orientation = 0;
#else
	orientation = 1;
#endif
#endif
	// Init LCD control bus and LCD Data bus
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
#ifndef HW_VER_3
	LCD_CTRL_PORT->BSRR = LCD_RD | LCD_RST | LCD_WR | LCD_RS | LCD_CS;
	GPIO_InitStructure.GPIO_Pin	= LCD_RD | LCD_RST | LCD_WR | LCD_RS | LCD_CS;
#else
	LCD_CTRL_PORT2->BSRR = LCD_RST | LCD_RS | LCD_CS;
	GPIO_InitStructure.GPIO_Pin	= LCD_RST | LCD_RS | LCD_CS;
	GPIO_Init(LCD_CTRL_PORT2, &GPIO_InitStructure);
	LCD_CTRL_PORT->BSRR = LCD_RD | LCD_WR;
	GPIO_InitStructure.GPIO_Pin = LCD_RD | LCD_WR;
#endif
GPIO_Init(LCD_CTRL_PORT, &GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin	= LCD_DATA_MASK;
GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);

#ifndef HW_VER_3
	// Init Encoder lines
	GPIO_InitStructure.GPIO_Pin	= ENC_A | ENC_B;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU;
	GPIO_Init(ENC_PORT, &GPIO_InitStructure);
#else
	// Init touchscreen lines
	GPIO_InitStructure.GPIO_Pin	= TS_YU | TS_YD;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(TS_PORT, &GPIO_InitStructure);
	TS_PORT->BRR = TS_YU | TS_YD;
	GPIO_InitStructure.GPIO_Pin	= TS_XL | TS_XR;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(TS_PORT, &GPIO_InitStructure);
#endif

#if defined(HW_VER_1)
	AFIO->EXTICR[2] &= ~(AFIO_EXTICR3_EXTI9);
	AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI9_PA; //channel 9 EXTI connected to PA9
#elif defined(HW_VER_2)
	AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0);
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PB; //channel 0 EXTI connected to PB0
#endif
#ifndef HW_VER_3
	EXTI->FTSR |= ENC_A;	//falling
	EXTI->PR = ENC_A;		//clear flag
	EXTI->IMR |= ENC_A;		//enable interrupt from channel 0
	NVIC_EnableIRQ(ENC_IRQn);
#endif
	// Init Button
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin	= BUTTONS_A_MSK;
	GPIO_Init(BTN_PORTA, &GPIO_InitStructure);
#ifndef HW_VER_3
	GPIO_InitStructure.GPIO_Pin	= BUTTONS2_MSK;
	GPIO_Init(BTN_PORT2, &GPIO_InitStructure);
#endif

	// PWM signals init
	GPIO_InitStructure.GPIO_Pin	= BRIGHTNES_PIN | BUZZER_PIN;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
	GPIO_Init(PWM_PORT, &GPIO_InitStructure);

	// Configure pins for SPI
	GPIO_InitStructure.GPIO_Pin = SPI_MOSI | SPI_MISO | SPI_SCK | SPI_CS;
	GPIO_Init(SPI_PORT, &GPIO_InitStructure);

	// Configure pins for I2C2
	GPIO_InitStructure.GPIO_Pin		= I2C_SCL | I2C_SDA;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_OD;
	GPIO_Init(I2C_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BOOT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(BOOT_PORT, &GPIO_InitStructure);
	BOOT_PORT->BRR = BOOT_PIN;

#ifndef HW_VER_2
	//for test output
	GPIO_InitStructure.GPIO_Pin		= TEST_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_Init(TEST_PORT, &GPIO_InitStructure);
#endif
}
/***********************************************************
 Timer for buttons and touchscreen reading
***********************************************************/
static void Timer_Btn_init(void)
{
	RCC->APB2ENR |= Btn_RCC_ENR;	// up counter, only update interrupt
	Timer_Btn->CNT = 0;
	Timer_Btn->PSC = 720;	// Fclk = 100kHz
	Timer_Btn->ARR = 2000;	// 20 ms
	Timer_Btn->DIER = TIM_DIER_UIE;
	NVIC_SetPriority(Btn_IRQ, 1);
	NVIC_EnableIRQ(Btn_IRQ);
	Timer_Btn->CR1 |= TIM_CR1_CEN;	//timer on
}
/***********************************************************
 Timer for encoder anti-bounce input and for touchscreen
***********************************************************/
static void Timer_Delay_init(void)
{
	RCC->APB1ENR |= Delay_RCC_ENR;	// up counter, only update interrupt
#ifndef HW_VER_3
	Timer_Del->PSC = 72;	// Fclk = 1MHz
	Timer_Del->ARR = 1000;	// delay, us
#else
	Timer_Del->PSC = 7200;	// Fclk = 10kHz
	Timer_Del->ARR = 1000;	// delay, 100 ms
#endif
	Timer_Del->DIER = TIM_DIER_UIE;
	Timer_Del->EGR = TIM_EGR_UG;
	NVIC_SetPriority(Delay_IRQ, 1);
	NVIC_EnableIRQ(Delay_IRQ);
}
/***********************************************************
 Timer for buzzer duration
***********************************************************/
static void Timer_Duration_init(void)
{
	RCC->APB1ENR |= Dur_RCC_ENR;	// up counter, only update interrupt
	Timer_D->PSC = 72;		// Fclk = 1MHz
	Timer_D->CNT = 0;
	Timer_D->ARR = 1000;	// 1ms period
	Timer_D->DIER = TIM_DIER_UIE;
	Timer_D->EGR = TIM_EGR_UG;
	NVIC_SetPriority(Duration_IRQ, 1);
	NVIC_EnableIRQ(Duration_IRQ);
}
/***********************************************************
 Timer for PWM (BUZZER, BRIGHTNES)
***********************************************************/
static void Timer_PWM_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	uint16_t PrescalerValue = 0;

	// TIM clock enable
  	RCC_APB1PeriphClockCmd(PWM_RCC_ENR, ENABLE);

#ifdef HW_VER_1
	AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;
#endif

	// Compute the prescaler value
	PrescalerValue = (uint16_t) (SystemCoreClock / 256000);	//frequency = 1kHz for Period = 255

	// Time base configuration
	TIM_TimeBaseStructure.TIM_Period = 255;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(Timer_P, &TIM_TimeBaseStructure);

	// PWM1 Mode configuration for Buzzer
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
#ifndef HW_VER_3
	TIM_OC4Init(Timer_P, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(Timer_P, TIM_OCPreload_Enable);
#else
	TIM_OC1Init(Timer_P, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(Timer_P, TIM_OCPreload_Enable);
#endif

	// PWM1 Mode configuration for LCD BRIGHTNES
	TIM_OCInitStructure.TIM_Pulse = LCD_BRIGHTNES;
#ifndef HW_VER_3
	TIM_OC3Init(Timer_P, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(Timer_P, TIM_OCPreload_Enable);
#else
	TIM_OC2Init(Timer_P, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(Timer_P, TIM_OCPreload_Enable);
#endif

	TIM_ARRPreloadConfig(Timer_P, ENABLE);

	TIM_Cmd(Timer_P, ENABLE);
}
/***********************************************************
***********************************************************/
#ifdef USE_SPI
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
	NVIC_SetPriority(SPI_IRQ, 0);
	NVIC_EnableIRQ(SPI_IRQ);
}
#endif
/***********************************************************
***********************************************************/
#ifdef USE_I2C
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
#endif
/***********************************************************
***********************************************************/
#ifdef HW_VER_3
void ADC_init()
{
	ADC_InitTypeDef ADC_InitStructure;

	// Configure the clocks
	RCC_ADCCLKConfig (RCC_PCLK2_Div8);
	RCC_APB2PeriphClockCmd(ADC_RCC, ENABLE);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Cmd(ADC1, ENABLE);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1));
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
#endif
/***********************************************************
***********************************************************/
void Global_Init(void)
{
	SystemInit();
	delay_init();
	GPIO_init();
	Timer_Delay_init();
	Timer_Duration_init();
	Timer_PWM_init();
#ifdef USE_SPI
	SPI_init();
#endif
#ifdef USE_I2C
	I2C_init();
#endif
#ifdef HW_VER_3
	ADC_init();
	restoreSettings();
#endif
	LCD_Init();
	Timer_Btn_init();
}
