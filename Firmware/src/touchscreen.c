#include "LCD.h"

#ifdef HW_VER_3

#include "settings.h"
#include "stm32f10x_adc.h"
#include "touchscreen.h"

uint8_t touchstate = 0;
uint8_t ts_pressed = 0;
uint8_t ts_x = 0, ts_y = 0;
uint16_t adc;
uint16_t adc_dY = 0;
uint8_t adc_y_calibrate = 0;
uint8_t delay_cnt = 0;
#if defined(LCD320x240)
uint8_t line_3[CHARS_PER_LINE] = {' ',' ',' ','[',0x1a,0x1a,0x1a,']',' ',' ',' ',' ','[',0x16,0x16,0x16,']',' ',' ',' '};	//[---] [+++]
//uint8_t line_2[CHARS_PER_LINE] = {' ',' ',' ','[',0x17,0x18,0x19,']',' ',' ',' ',' ','[',0x13,0x14,0x15,']',' ',' ',' '};	//[--] [++]
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

void print_dec(uint16_t dec)
{
	uint16_t t;
	if (dec > 10000) {t = dec / 10000;	LCD_DrawChar('0' + t);	dec -= t * 10000;}
	if (dec > 1000)	{t = dec / 1000;	LCD_DrawChar('0' + t);	dec -= t * 1000;}
	if (dec > 100)	{t = dec / 100;		LCD_DrawChar('0' + t);	dec -= t * 100;}
	if (dec > 10)	{t = dec / 10;		LCD_DrawChar('0' + t);	dec -= t * 10;}
	LCD_DrawChar('0' + dec);
}
//----------------------------------------------------------------------------
void Calibrate_touch()
{
	uint8_t i, j, char1, char2;

	Timer_Btn->CR1 &= ~TIM_CR1_CEN;	//timer off
	if (!adc_y_calibrate)
	{	//first press
		LCD_ClearScreen();
		LCD_Set_TextColor(CURSOR_TEXT_COLOR, CURSOR_BACK_COLOR);
		LCD_SetCursor(0, 0);	LCD_PutStrig("Press =>");
		LCD_Set_TextColor(White, BackColor);
		LCD_SetCursor(9, 0);	LCD_DrawChar('+');
		adc_y_calibrate = 1;
	}
	else if (adc_y_calibrate == 2)
	{
		LCD_Set_TextColor(White, BackColor);
		LCD_SetCursor(0, 0);
		for (j = 0; j < 10; j++)	LCD_DrawChar(' ');
		LCD_Set_TextColor(CURSOR_TEXT_COLOR, CURSOR_BACK_COLOR);
		LCD_SetCursor(0, 9);	LCD_PutStrig("Press =>");
		LCD_Set_TextColor(White, BackColor);
		LCD_SetCursor(9, 9);	LCD_DrawChar('+');
		adc_y_calibrate = 3;
	}
	else if (adc_y_calibrate == 4)
	{
		LCD_Set_TextColor(White, BackColor);
		LCD_SetCursor(0, 9);
		for (j = 0; j < 10; j++)	LCD_DrawChar(' ');
		adc_y_calibrate = 5;
		adc_dY = Settings.adc_Ymax - Settings.adc_Ymin;
		Settings.adc_1_line = adc_dY / (TEXT_LINES - 1);
		LCD_SetCursor((CHARS_PER_LINE / 2) - 5, 4);	LCD_PutStrig("Ymin = ");	print_dec(Settings.adc_Ymin);
		LCD_SetCursor((CHARS_PER_LINE / 2) - 5, 5);	LCD_PutStrig("Ymax = ");	print_dec(Settings.adc_Ymax);
		saveSettings();
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
			{
				char1 = char2 = 0x1b;
			}
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
			if (ts_y == cur_line) goto go_to_menu;
			break;
		case EDIT_SCREEN:
			if (ts_y <= MIDDLE_Y)
				goto go_to_menu;
			else
			{
				if 		(ts_y == (MIDDLE_Y + 2))	ed = 20;
				else if (ts_y == (MIDDLE_Y + 4))	ed = 2;
				else	break;
				if (!ts_x)	ed = -ed;
				encdiff = ed;
				if (delay_cnt == 10)	delay_cnt = 8;	//autorepeat time ~ 0.2s
			}
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
go_to_menu:
				if (protocol == Smoothie)	buttons = BUTTON_SELECT;
				else						buttons = EN_C;
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
			break;
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
			else if (adc < Settings.adc_Ymax)	ts_y = (uint8_t)((adc /*- Settings.adc_Ymin*/) / Settings.adc_1_line);
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
			if ((ts_pressed == 1) || (delay_cnt == 10))	Emulate_Encoder();
	}
}
#endif
