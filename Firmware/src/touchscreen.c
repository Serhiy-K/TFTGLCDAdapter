#include "LCD.h"

#ifdef HW_VER_3

#include "debug_functions.h"
#include "settings.h"
#include "stm32f10x_adc.h"
#include "touchscreen.h"

uint8_t touchstate = 0;
uint8_t ts_pressed = 0;
uint8_t ts_x = 0, ts_y = 0;
uint16_t adc;
uint8_t adc_y_calibrate = 0;
uint8_t delay_cnt = 0;
#if defined(LCD320x240)
uint8_t line_3[CHARS_PER_LINE] = {' ',' ',' ','[',0x1a,0x1a,0x1a,']',' ',' ',' ',' ','[',0x16,0x16,0x16,']',' ',' ',' '};	//[---] [+++]
//uint8_t line_2[CHARS_PER_LINE] = {' ',' ',' ','[',0x17,0x18,0x19,']',' ',' ',' ',' ','[',0x13,0x14,0x15,']',' ',' ',' '};	//[--]  [++]
uint8_t line_1[CHARS_PER_LINE] = {' ',' ',' ','[',0x12,0x1a,0x12,']',' ',' ',' ',' ','[',0x12,0x16,0x12,']',' ',' ',' '};	//[ - ] [ + ]
#elif defined(LCD400x240)
uint8_t line_3[CHARS_PER_LINE] = {' ',' ',' ',' ',' ','[',0x1a,0x1a,0x1a,']',' ',' ',' ',' ','[',0x16,0x16,0x16,']',' ',' ',' ',' '};
//uint8_t line_2[CHARS_PER_LINE] = {' ',' ',' ',' ',' ','[',0x17,0x18,0x19,']',' ',' ',' ',' ','[',0x13,0x14,0x15,']',' ',' ',' ',' '};
uint8_t line_1[CHARS_PER_LINE] = {' ',' ',' ',' ',' ','[',0x12,0x1a,0x12,']',' ',' ',' ',' ','[',0x12,0x16,0x12,']',' ',' ',' ',' '};
#endif
uint8_t screen_mode = START_SCREEN;
uint8_t cur_line = 0; //for menu
uint8_t cur_x = 0;

extern uint8_t orientation;
extern int8_t  encdiff;
extern uint8_t buttons;
extern uint8_t protocol;

//----------------------------------------------------------------------------
void Calibrate_touch()
{
	uint8_t i, j, char1, char2;

	Timer_Btn->CR1 &= ~TIM_CR1_CEN;	//timer off
	if (!adc_y_calibrate)
	{	//first press
		LCD_ClearScreen();
		LCD_Set_TextColor(CURSOR_TEXT_COLOR, CURSOR_BACK_COLOR);
		LCD_PutStrig_XY(0, 0, "Press =>");
		LCD_FillRect(LCDXMAX / 2 - CHAR_HEIGTH, CHAR_HEIGTH - 1, LCDXMAX / 2 + CHAR_HEIGTH, CHAR_HEIGTH - 1, White);
		LCD_FillRect(LCDXMAX / 2, 0, LCDXMAX / 2, CHAR_HEIGTH * 2, White);
#ifdef CALIBR_DEBUG_INFO
		LCD_Set_TextColor(White, BackColor);
		LCD_PutStrig_XY(1, 2, "Old Ymin =");	LCD_SetCursor(12, 2);	print_dec(Settings.adc_Ymin);
#endif
		adc_y_calibrate = 1;
	}
	else if (adc_y_calibrate == 2)
	{
#ifdef CALIBR_DEBUG_INFO
		LCD_PutStrig_XY(1, 3, "New Ymin =");	LCD_SetCursor(12, 3);	print_dec(Settings.adc_Ymin);
		LCD_PutStrig_XY(1, 4, "Old Ymax =");	LCD_SetCursor(12, 4);	print_dec(Settings.adc_Ymax);
		LCD_Set_TextColor(CURSOR_TEXT_COLOR, CURSOR_BACK_COLOR);
#endif
		LCD_FillRect(0, 0, LCDXMAX / 2 + CHAR_HEIGTH, CHAR_HEIGTH * 2, BackColor);
		LCD_PutStrig_XY(0, 9, "Press =>");
		LCD_Set_TextColor(White, BackColor);
		LCD_FillRect(LCDXMAX / 2 - CHAR_HEIGTH, LCDYMAX - CHAR_HEIGTH + 1, LCDXMAX / 2 + CHAR_HEIGTH, LCDYMAX - CHAR_HEIGTH + 1, White);
		LCD_FillRect(LCDXMAX / 2, LCDYMAX - CHAR_HEIGTH * 2, LCDXMAX / 2, LCDYMAX - 1, White);
		adc_y_calibrate = 3;
	}
	else if (adc_y_calibrate == 4)
	{
		LCD_FillRect(0, LCDYMAX - 1 - CHAR_HEIGTH * 2, LCDXMAX / 2 + CHAR_HEIGTH, LCDYMAX - 1, BackColor);
		Settings.adc_1_line = (Settings.adc_Ymax - Settings.adc_Ymin) / (TEXT_LINES - 2);
#ifdef CALIBR_DEBUG_INFO
		LCD_PutStrig_XY(1, 5, "New Ymax =");	LCD_SetCursor(12, 5);	print_dec(Settings.adc_Ymax);
		LCD_PutStrig_XY((CHARS_PER_LINE - 13) / 2, 7, "Press to test");
		LCD_PutStrig_XY((CHARS_PER_LINE - 17) / 2, 8, "Then reboot panel");
#else
		LCD_PutStrig_XY((CHARS_PER_LINE - 13) / 2, MIDDLE_Y, "Press to test");
		LCD_PutStrig_XY((CHARS_PER_LINE - 17) / 2, MIDDLE_Y + 1, "Then reboot panel");
#endif
		saveSettings();
		adc_y_calibrate = 5;
	}
	else if (adc_y_calibrate == 6)
	{
		for (i = 0; i < TEXT_LINES; i++)
		{
			LCD_SetCursor(0, i);
			if (ts_y == i)
			{
				if (!ts_x)	{char1 = 0xdb; char2 = 0x1b;}
				else		{char1 = 0x1b; char2 = 0xdb;}
			}
			else
				char1 = char2 = 0x1b;
			for (j = 0; j < CHARS_PER_LINE / 2 ; j++)	LCD_DrawChar(char1);
			for (j = CHARS_PER_LINE / 2; j < CHARS_PER_LINE; j++)	LCD_DrawChar(char2);
		}
	}
	Timer_Btn->CR1 |= TIM_CR1_CEN;	//timer on
}
//----------------------------------------------------------------------------
void Emulate_Encoder()
{
	int8_t ed = 0;
	switch (screen_mode)
	{
		case MENU_SCREEN:
			encdiff = (ts_y - cur_line) * 2;
			if (encdiff == 0)	goto press_enc;
			break;
		case EDIT_SCREEN:
			if 		(ts_y <= MIDDLE_Y)	goto press_enc;
			if 		(ts_y == (MIDDLE_Y + 2))	ed = 20;
			else if (ts_y == (MIDDLE_Y + 4))	ed = 2;
			else	break;
			if (!ts_x)	ed = -ed;
			encdiff = ed;
			if (delay_cnt == 10)	delay_cnt = 8;	//autorepeat time ~ 0.2s
			break;
		case SELECT_SCREEN:
			if (cur_x != ts_x)
			{
				cur_x = ts_x;
				if (ts_x)	encdiff = 2;
				else		encdiff = -2;
			}
			else
			{
				cur_x = 0;
		case MAIN_SCREEN:
press_enc:
#ifndef ONLY_MARLIN
				if (protocol == Smoothie)
					buttons = BUTTON_SELECT;
				else
#endif
					buttons = EN_C;
			}
			break;
		case MAIN_UBL:
			if (!ts_x && (ts_y == 8))
			{
				buttons = EN_D;
				break;
			}
		case EDIT_UBL:
			if (ts_x)
			{
				if 		(ts_y == 5)	encdiff = 2;
				else if (ts_y == 7)	encdiff = -2;
				else	buttons = EN_C;
				if (delay_cnt == 10)	delay_cnt = 8;
			}
			else
				buttons = EN_C;
			break;
		case START_SCREEN:
			Calibrate_touch();
	}
}
//----------------------------------------------------------------------------
void Read_Touch()
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;

	switch (touchstate)
	{
		case 0:
			if (!(TS_PORT->IDR & TS_XL))
			{	//touch is pressed
				if (screen_mode == MAIN_SCREEN)
					Emulate_Encoder();
				else
				{	//need get XY position
					if (ts_pressed < 2) ts_pressed++;
					touchstate = 1;
					GPIO_InitStructure.GPIO_Pin	= TS_XL | TS_XR;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					GPIO_Init(TS_PORT, &GPIO_InitStructure);
#if !defined(SET_ORIENT_RIGHT) && !defined(SET_ORIENT_LEFT)
					if (!orientation)	TS_PORT->BSRR = TS_YU;
					else				TS_PORT->BSRR = TS_YD;
#else
#ifdef SET_ORIENT_RIGHT
					TS_PORT->BSRR = TS_YU;
#else
					TS_PORT->BSRR = TS_YD;
#endif
#endif	// !SET_ORIENT_RIGHT, !SET_ORIENT_LEFT
					ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5);	//TS_XL
					if (!(Timer_Del->CR1 & TIM_CR1_CEN) && (screen_mode > SELECT_SCREEN))
					{	//start delay for autorepeat
						delay_cnt = 0;
						Timer_Del->CNT = 0;
						Timer_Del->CR1 |= TIM_CR1_CEN;	//delay on
					}
				}
			}
			else
			{
				Timer_Del->CR1 &= ~TIM_CR1_CEN;	//autorepeat delay off
				delay_cnt = 0;
				ts_pressed = 0;
				buttons = 0;
				if		(adc_y_calibrate == 1)	adc_y_calibrate = 2;
				else if (adc_y_calibrate == 3)	adc_y_calibrate = 4;
				else if (adc_y_calibrate == 5)	adc_y_calibrate = 6;
			}
			break;
		case 1:	//measure Y
			adc = ADC_GetConversionValue(ADC1);
			if		(adc_y_calibrate == 2) Settings.adc_Ymin = adc;
			else if (adc_y_calibrate == 4) Settings.adc_Ymax = adc;
			if		(adc < Settings.adc_Ymin)	ts_y = 0;
			else if (adc < Settings.adc_Ymax)	ts_y = 1 + (uint8_t)((adc - Settings.adc_Ymin) / Settings.adc_1_line);
			else								ts_y = TEXT_LINES - 1;
			if (screen_mode == MENU_SCREEN)
				goto no_tsx;
			touchstate = 2;
			GPIO_InitStructure.GPIO_Pin	= TS_YU | TS_YD;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
			GPIO_Init(TS_PORT, &GPIO_InitStructure);
			ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5);	//TS_YU
			GPIO_InitStructure.GPIO_Pin	= TS_XR | TS_XL;
			GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
			GPIO_Init(TS_PORT, &GPIO_InitStructure);
#if !defined(SET_ORIENT_RIGHT) && !defined(SET_ORIENT_LEFT)
			if (!orientation)	{TS_PORT->BSRR = TS_XL;	TS_PORT->BRR = TS_XR;}
			else				{TS_PORT->BSRR = TS_XR;	TS_PORT->BRR = TS_XL;}
#else
#ifdef SET_ORIENT_RIGHT
			TS_PORT->BSRR = TS_XL;	TS_PORT->BRR = TS_XR;
#else
			TS_PORT->BSRR = TS_XR;	TS_PORT->BRR = TS_XL;
#endif
#endif	// !SET_ORIENT_RIGHT, !SET_ORIENT_LEFT
			break;
		case 2:	//measure X
			adc = ADC_GetConversionValue(ADC1);
			if (adc > ADC_X_middle)	ts_x = 1;
			else					ts_x = 0;
no_tsx:
			touchstate = 0;
			GPIO_InitStructure.GPIO_Pin	= TS_YU | TS_YD;
			GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
			GPIO_Init(TS_PORT, &GPIO_InitStructure);
			TS_PORT->BRR = TS_YU | TS_YD;
			GPIO_InitStructure.GPIO_Pin	= TS_XL | TS_XR;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			GPIO_Init(TS_PORT, &GPIO_InitStructure);
			if ((screen_mode == MENU_SCREEN) || (ts_pressed == 1) || (delay_cnt == 10))	Emulate_Encoder();
	}
}
#endif
