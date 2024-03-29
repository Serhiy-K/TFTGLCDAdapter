#include "stm32f10x_i2c.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "debug_functions.h"
#include "LCD.h"
#include "Pictures.h"
#include "protocol.h"
#include "systick.h"
#ifdef HW_VER_3
#include "settings.h"
#include "touchscreen.h"
#endif

#define	TTO		0	// offset for text termo
#define	TMO		20	// offset for measured termo
#define	TSO		40	// offset for setted termo
//for one extruder config
#define HEO		1
#define	B1O		6
#define F1O		11
#define H1O		15
//for multy-extruders config
#define HE1O	0
#define HE2O	4
#define HE3O	8
#define	B3O		13
#define F3O		17

#define T_L_LINE	CHARS_PER_LINE * 5

#define pic_Ymin	LCDYMAX - 48
//for one extruder config
#define	pic1_Xmin1	CHAR_WIDTH * HEO	//HE
#define	pic2_Xmin1	CHAR_WIDTH * B1O	//BED
#define	pic3_Xmin1	CHAR_WIDTH * F1O	//FAN
#define pic4_Xmin1	CHAR_WIDTH * H1O	//HEAT
//for multy-extruders config
#define	pic1_Xmin	HE1O				//HE1
#define	pic2_Xmin	CHAR_WIDTH * HE2O	//HE2
#define	pic3_Xmin	CHAR_WIDTH * HE3O	//HE3
#define	pic4_Xmin	CHAR_WIDTH * B3O	//BED
#define	pic5_Xmin	CHAR_WIDTH * F3O	//FAN
//for laser/spindle
#define pic_COOL	CHAR_WIDTH * 1
#define pic_FLOW	CHAR_WIDTH * 6
#define pic_ILAZ	CHAR_WIDTH * 11
#define pic_CUTT	CHAR_WIDTH * 16

#ifdef	LCD320x240
#define pic6_Xmin	pic5_Xmin	//HEAT
#define pic6_Ymin	pic_Ymin - (CHAR_HEIGTH * 3)
#endif
#ifdef LCD400x240
#define pic6_Xmin	CHAR_WIDTH * 21
#define pic6_Ymin	pic_Ymin
#endif

//ASCII pseudographic symbols
#define TLC		218	//top left corner
#define TRC		191	//top right corner
#define BRC		217	//bottom right corner
#define BLC		192 //bottom left corner
#define GL		196	//gorizontal line
#define VL		179	//vertical line

#define FB_SIZE	(CHARS_PER_LINE * TEXT_LINES + 2)	//text area + leds + pics

// data[FB_SIZE - 1] - leds
// data[FB_SIZE - 2] - pics
uint8_t data[2][FB_SIZE];
uint8_t datat[60] = {' '};
uint8_t lines[TEXT_LINES] = {0};
uint8_t clr_lines[TEXT_LINES] = {0};

uint8_t in_buf = 0;
uint8_t out_buf = 0;

uint8_t cmd = 0;
int16_t pos = -1;
uint8_t toread = 0;
uint8_t new_command = 0;
uint8_t temps = 0;
uint8_t laser = 0;
uint16_t row_offset[2] = {0};
uint8_t progress_cleared = 0;
uint8_t protocol = Smoothie;
uint8_t buzcnt = 0;
uint8_t buzcntcur = 0;
uint16_t freq[MAX_FREQS] = {0}, duration[MAX_FREQS] = {0}; //different tones
uint8_t pics = 0;
uint8_t grid_size_x, grid_size_y;
uint16_t grid_x[20], grid_y[20];
uint16_t dot_pos_x, dot_pos_y;
uint8_t UBL_first_time = 1;
int8_t  encdiff = 0;
uint8_t new_buf = 0;
uint8_t USE_UBL = 0;
uint8_t init = 0;
uint8_t buttons = 0;

void Print_Line(uint8_t row);

//----------------------------------------------------------------------------
uint8_t New_cmd() {return new_command;}
//----------------------------------------------------------------------------
//For Smoothieware
//----------------------------------------------------------------------------
#ifndef ONLY_MARLIN
void Move_Text()
{
	uint8_t i, to;
	for (i = 1; i < 13; i++)
	{
		//scan first line for temperatures
		if (data[out_buf][i] == ':')
		{
			if (data[out_buf][i+1] == '0')
			{
				data[out_buf][i+1] = ' ';	if (data[out_buf][i+2] == '0')	data[out_buf][i+2] = ' ';
			}
			if (data[out_buf][i+5] == '0')
			{
				data[out_buf][i+5] = ' ';	if (data[out_buf][i+6] == '0')	data[out_buf][i+6] = ' ';
			}
			
			switch (data[out_buf][i-1])
			{
			case 'T':	//by default for hotend1
			case 'E':	//extruder
			case 'H':	//hotend
				if ((data[out_buf][i+10] == '1') || (data[out_buf][i+10] == '2'))
				{
					temps = 3;
					to =TTO + HE1O;	datat[to++] = 'H';	datat[to++] = 'E';	datat[to] = '1';
					to =TMO + HE1O;	datat[to++] = data[out_buf][i+1];	datat[to++] = data[out_buf][i+2];	datat[to++] = data[out_buf][i+3];
					to =TSO + HE1O;	datat[to++] = data[out_buf][i+5];	datat[to++] = data[out_buf][i+6];	datat[to++] = data[out_buf][i+7];
				}
				else
				{	//when first step and hotend present
					temps = 1;
					to = TTO + HEO + 1;	datat[to++] = 'H';	datat[to] = 'E';
					to = TMO + HEO;	datat[to++] = data[out_buf][i+1];	datat[to++] = data[out_buf][i+2];	datat[to] = data[out_buf][i+3];
					to = TSO + HEO;	datat[to++] = data[out_buf][i+5];	datat[to++] = data[out_buf][i+6];	datat[to] = data[out_buf][i+7];
				}
				break;
			case 'B':
				if (temps == 1)
				{	//when first step and one hotend
					to = TTO + B1O;	datat[to++] = 'B';	datat[to++] = 'E';	datat[to] = 'D';
					to = TMO + B1O;	datat[to++] = data[out_buf][i+1];	datat[to++] = data[out_buf][i+2];	datat[to] = data[out_buf][i+3];
					to = TSO + B1O;	datat[to++] = data[out_buf][i+5];	datat[to++] = data[out_buf][i+6];	datat[to] = data[out_buf][i+7];
				}
				else
				{
					to = TTO + B3O;	datat[to++] = 'B';	datat[to++] = 'E';	datat[to++] = 'D';
					to = TMO + B3O;	datat[to++] = data[out_buf][i+1];	datat[to++] = data[out_buf][i+2];	datat[to] = data[out_buf][i+3];
					to = TSO + B3O;	datat[to++] = data[out_buf][i+5];	datat[to++] = data[out_buf][i+6];	datat[to] = data[out_buf][i+7];
				}
				break;
			case '1':	//by default for hotend2
				to = TTO + HE2O;	datat[to++] = 'H';	datat[to++] = 'E';	datat[to] = '2';
				to = TMO + HE2O;	datat[to++] = data[out_buf][i+1];	datat[to++] = data[out_buf][i+2];	datat[to] = data[out_buf][i+3];
				to = TSO + HE2O;	datat[to++] = data[out_buf][i+5];	datat[to++] = data[out_buf][i+6];	datat[to] = data[out_buf][i+7];
				break;
			case '2':	//by default for hotend3
				to = TTO + HE3O;	datat[to++] = 'H';	datat[to++] = 'E';	datat[to] = '3';
				to = TMO + HE3O;	datat[to++] = data[out_buf][i+1];	datat[to++] = data[out_buf][i+2];	datat[to] = data[out_buf][i+3];
				to = TSO + HE3O;	datat[to++] = data[out_buf][i+5];	datat[to++] = data[out_buf][i+6];	datat[to] = data[out_buf][i+7];
				break;
			}
		}
	}
	//move up lines 2 and 3
	for (i = 0; i < CHARS_PER_LINE * 2; i++)	data[out_buf][i] = data[out_buf][i + CHARS_PER_LINE];

	//progressbar
	data[out_buf][CHARS_PER_LINE * 2] = '%';
	i = data[out_buf][CHARS_PER_LINE + 18] - '0';	//fixed position
	if (data[out_buf][CHARS_PER_LINE + 17] != ' ')
	{
		i += (data[out_buf][CHARS_PER_LINE + 17] - '0') * 10;
		if (data[out_buf][CHARS_PER_LINE + 16] == '1')	i = 100;
	}
	data[out_buf][CHARS_PER_LINE * 2 + 1] = i;

	//fan percent present
	if (data[out_buf][CHARS_PER_LINE * 4] == '%')
	{
		if (!data[out_buf][CHARS_PER_LINE * 4 + 1])	//in fact FAN is in off state, so FAN icon depend on fan percent
			data[out_buf][FB_SIZE - 2] &= ~PIC_FAN;

		if (temps == 1)
		{
			to = TTO + F1O;	datat[to++] = 'F';	datat[to++] = 'A';	datat[to] = 'N';
			datat[TMO + F1O + 1] = '%';
			to = TSO + F1O;
			goto FAN_P;
		}
		else if (temps == 3)
		{
			to = TTO + F3O;	datat[to++] = 'F';	datat[to++] = 'A';	datat[to] = 'N';
			datat[TMO + F3O + 1] = '%';
			to = TSO + F3O;
FAN_P:
			if (data[out_buf][CHARS_PER_LINE * 4 + 1] == 100)
			{
				datat[to++] = '1';	datat[to++] = '0';	datat[to] = '0';
			}
			else if (!data[out_buf][CHARS_PER_LINE * 4 + 1])
			{
				datat[to++] = ' ';	datat[to++] = '0';	datat[to] = ' ';
			}
			else
			{
				datat[to++] = ' ';
				i = data[out_buf][CHARS_PER_LINE * 4 + 1] / 10;
				if (!i)
				{
					datat[to++] = data[out_buf][CHARS_PER_LINE * 4 + 1] + '0';
					datat[to] = ' ';
				}
				else
				{
					datat[to++] = i + '0';
					datat[to] = (data[out_buf][CHARS_PER_LINE * 4 + 1] - i * 10) + '0';
				}
			}
		}
		data[out_buf][CHARS_PER_LINE * 4] = data[out_buf][CHARS_PER_LINE * 4 + 1] = ' ';	//clear percent symbol and data
	}
}
//----------------------------------------------------------------------------
void Check_for_edit_mode()
{	//on edit param screen line 2 is free and line 3 has text
	uint8_t i;
	//check line 2
	for (i = 0; i < CHARS_PER_LINE; i++)
	{
		if (data[out_buf][CHARS_PER_LINE + i] != ' ')
		{	//line 2  has text
#ifdef HW_VER_3
			screen_mode = MENU_SCREEN;
#endif
			return;
		}
	}
	//check line 3
	if (data[out_buf][CHARS_PER_LINE * 2] == '#')	return;
	for (i = CHARS_PER_LINE * 2; i < CHARS_PER_LINE * 3; i++)
	{
		if (data[out_buf][i] != ' ')
		{	//line 3 has text
			for (i = CHARS_PER_LINE * 3 - 1; i > CHARS_PER_LINE * 2; i--)	data[out_buf][i] = data[out_buf][i - 1];	//shift text right
			data[out_buf][CHARS_PER_LINE * 2] = '#';	//put marker for new colors
#ifdef HW_VER_3
			screen_mode = EDIT_SCREEN;
#endif
			return;
		}
	}
}
//----------------------------------------------------------------------------
void Set_Leds()
{
	uint8_t leds;

	leds = data[in_buf][FB_SIZE - 1];
	leds &= LED_MASK;

#ifdef USE_3DPRINTER
#ifndef WITHOUT_HEAT_ICO
	if (leds & LED_HOT)
	{
		if (temps == 1)	LCD_Draw_Picture (pic4_Xmin1, pic_Ymin, &heat_48x48[0]);
		else			LCD_Draw_Picture (pic6_Xmin, pic6_Ymin, &heat_48x48[0]);
	}
	else
	{
		if (temps == 1)	LCD_Clear_Picture(pic4_Xmin1, pic_Ymin);
		else			LCD_Clear_Picture(pic6_Xmin, pic6_Ymin);
	}
#endif
#endif
}
#endif
//----------------------------------------------------------------------------
// For Marlin
//----------------------------------------------------------------------------
void Clear_Screen()
{	// clear only output buffer
	uint16_t i;
	new_command = 0;
	for (i = 0; i < (FB_SIZE - 2); i++)	data[out_buf][i] = ' ';
	for (i = 0; i < TEXT_LINES; i++)	lines[i] = 0;
#ifdef HW_VER_3
	for (i = 0; i < TEXT_LINES; i++)	clr_lines[i] = 1;
	screen_mode = MAIN_SCREEN;
#endif
	progress_cleared = 0;
	UBL_first_time = 1;
	LCD_Set_TextColor(White, Black);
}
//----------------------------------------------------------------------------
void UBL_Draw_Dots(uint8_t y)
{
	uint8_t i, j;
	uint8_t ymin = y * CHAR_HEIGTH;
	uint8_t ymax = ymin + CHAR_HEIGTH;

	for (i = 0; i < grid_size_y; i++)
	{
		if ((grid_y[i] >= ymin) && (grid_y[i] < ymax))
		{
			y = grid_y[i];
			for (j = 0; j < grid_size_x; j++)
			{
				if ((dot_pos_x == j) && (dot_pos_y == i))
					LCD_FillRect(grid_x[j] - 4, y - 4, grid_x[j] + 3, y + 3, Yellow);
				else
				{
					LCD_FillRect(grid_x[j] - 4, y - 4, grid_x[j] + 3, y + 3, Black);
					LCD_FillRect(grid_x[j] - 1, y - 1, grid_x[j] + 1, y + 1, White);
				}
			}
		}
	}
}
//----------------------------------------------------------------------------
void Print_Line_UBL(uint8_t row)
{
	uint16_t i, j;

	LCD_SetCursor(0, row);
	// draw frame and grid
	if (row == 0)
	{
		LCD_DrawChar(TLC);
		for (i = 1; i < 10; i++)	LCD_DrawChar(GL);
		LCD_DrawChar(TRC);
	}
	else if (row == 7)
	{
		LCD_DrawChar('+');
		for (i = 1; i < 10; i++)	LCD_DrawChar(GL);
		LCD_DrawChar(BRC);
	}
	else
	{
		LCD_DrawChar(VL);
		UBL_Draw_Dots(row);
		LCD_SetCursor(10, row);	LCD_DrawChar(VL);
	}
#ifdef HW_VER_3
	if ((screen_mode == EDIT_UBL) && (row > 2))
		return;
	else
#endif
	{
		i = row * CHARS_PER_LINE + 11;
		for (j = 11; j < CHARS_PER_LINE; j++)	LCD_DrawChar(data[out_buf][i++]);
	}
}
//----------------------------------------------------------------------------
void Draw_UBL_Screen()
{
/**
* Map screen:
* |/---------\ (00,00) |
* || . . . . | X:000.00|
* || . . . . | Y:000.00|
* || . . . . | Z:00.000|
* || . . . . |         |
* || . . . . |  [Z+]   | text for touchscreen
* || . . . . |         |
* |+---------/  [Z-]   | text for touchscreen
* |                    |
* |____________________| edit line for Z offset for encoder
*/
	uint8_t i;
	uint8_t pos_y;	//pos_y - inverted
	uint8_t step_x, step_y;

	USE_UBL = 1;

	grid_size_x = data[out_buf][1] >> 4;
	grid_size_y = data[out_buf][1] & 0x0f;

	step_x = ((CHAR_WIDTH * 9) - 8) / (grid_size_x - 1);
	step_y = ((CHAR_HEIGTH * 6) - 8) / (grid_size_y - 1);

	//calculate grid
	for (i = 0; i < grid_size_x; i++)	{grid_x[i] = CHAR_WIDTH + 4 + step_x * i;}
	for (i = 0; i < grid_size_y; i++)	{grid_y[i] = CHAR_HEIGTH + 4 + step_y * i;}

	i = 10;
	while (data[out_buf][++i] != ',');

	dot_pos_x = data[out_buf][i - 1] - '0';
	if (data[out_buf][i - 2] != '(')	//dot_pos_x >= 10
		dot_pos_x += (data[out_buf][i - 2] - '0') * 10;

	if (data[out_buf][i + 2] != ')')
	{	//pos_y >= 10
		pos_y = (data[out_buf][i + 1] - '0') * 10;
		pos_y += data[out_buf][i + 2] - '0';
	}
	else
		pos_y = data[out_buf][i + 1] - '0';

	dot_pos_y = grid_size_y - pos_y - 1;

	if (UBL_first_time)	{LCD_ClearScreen();	UBL_first_time = 0;}
	LCD_Set_TextColor(White, Black);
	for (i = 0; i < 8; i++)		Print_Line_UBL(i);
	Print_Line(8);
	Print_Line(9);
}
//----------------------------------------------------------------------------
void Print_Lines()
{
	uint8_t i;
new_line:
	new_command = 0;
	for (i = 0; i < TEXT_LINES; i++)
	{
		if (lines[i])	{Print_Line(i);	lines[i] = clr_lines[i] = 0;}
	}
#ifdef HW_VER_3
	if (!USE_UBL)
	{
		for (i = 0; i < TEXT_LINES; i++)
		{
			if (clr_lines[i])	{Print_Line(i);	clr_lines[i] = 0;}
		}
	}
#endif
	if (new_command == LCD_PUT)
		goto new_line;	//new line was received during output
}
//----------------------------------------------------------------------------
// Common
//----------------------------------------------------------------------------
uint8_t Read_Buttons()
{
	uint32_t b;

#if defined(HW_VER_1)
	b = BTN_PORTA->IDR & BUTTONS_A_MSK;
#ifndef ONLY_MARLIN
	if (protocol == Smoothie)
	{
		switch (b)
		{
			case (BUTTONS_A_MSK & ~ENC_BUT):		return BUTTON_SELECT;
			case (BUTTONS_A_MSK & ~BUTTON_PIN1):	return BUTTON_RIGHT;
			case (BUTTONS_A_MSK & ~BUTTON_PIN2):	return BUTTON_LEFT;
			case (BUTTONS_A_MSK & ~BUTTON_PIN3):	return BUTTON_UP;
		}
		b = BTN_PORT2->IDR;
		if (!(b & BUTTON_PIN4))	return BUTTON_DOWN;
	}
	else
#endif
	{
		switch (b)
		{
			case (BUTTONS_A_MSK & ~ENC_BUT):		return EN_C;
			case (BUTTONS_A_MSK & ~BUTTON_PIN1):	return EN_D;
			case (BUTTONS_A_MSK & ~BUTTON_PIN2):	return KILL;
		}
	}
#elif defined(HW_VER_2)
#ifndef ONLY_MARLIN
	if (protocol == Smoothie)
	{
		if(!(BTN_PORTA->IDR & ENC_BUT))	return BUTTON_SELECT;
		b = BTN_PORT2->IDR & BUTTONS2_MSK;
		switch (b)
		{
			case (BUTTONS2_MSK & ~BUTTON_PIN1):	return BUTTON_RIGHT;
			case (BUTTONS2_MSK & ~BUTTON_PIN2):	return BUTTON_LEFT;
			case (BUTTONS2_MSK & ~(BUTTON_PIN1 | BUTTON_PIN2)):	return BUTTON_UP;
		}
	}
	else
#endif
	{
		if (!(BTN_PORTA->IDR & ENC_BUT))	return EN_C;
		b = BTN_PORT2->IDR & BUTTONS2_MSK;
		if (b == (BUTTONS2_MSK & ~BUTTON_PIN1))	return EN_D;
		if (b == (BUTTONS2_MSK & ~BUTTON_PIN2))	return KILL;	//for I2C connection ??
	}
#elif defined(HW_VER_3)
	b = BTN_PORTA->IDR & BUTTONS_A_MSK;
#ifndef ONLY_MARLIN
	if (protocol == Smoothie)
	{
		switch (b)
		{
			case (BUTTONS_A_MSK & ~BUTTON_PIN1):	return BUTTON_RIGHT;
			case (BUTTONS_A_MSK & ~BUTTON_PIN2):	return BUTTON_LEFT;
			case (BUTTONS_A_MSK & ~BUTTON_PIN3):	return BUTTON_UP;
			case (BUTTONS_A_MSK & ~BUTTON_PIN4):	return BUTTON_DOWN;
		}
	}
	else
#endif
	{
		switch (b)
		{
			case (BUTTONS_A_MSK & ~BUTTON_PIN1):	return EN_D;
			case (BUTTONS_A_MSK & ~BUTTON_PIN2):	return KILL;
		}
	}
#endif
	return 0;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Print_Temps()
{
	#if defined(LCD320x240) && !defined(WITHOUT_HEAT_ICO)
		#define MX	16
	#else
		#define MX	20
	#endif
	uint8_t x;
	//main screen
#ifndef ONLY_MARLIN
	if (protocol == Smoothie)
	{
		LCD_SetCursor(0, 5);	for (x = 0; x < MX; x++)	LCD_DrawChar(datat[TTO + x]);
		LCD_SetCursor(0, 6);	for (x = 0; x < MX; x++)	LCD_DrawChar(datat[TMO + x]);
		LCD_SetCursor(0, 7);	for (x = 0; x < 20; x++)	LCD_DrawChar(datat[TSO + x]);
	}
	else
#endif
	{
		LCD_SetCursor(0, 5);	for (x = 0; x < MX; x++)	LCD_DrawChar(data[out_buf][T_L_LINE + x]);
		LCD_SetCursor(0, 6);	for (x = 0; x < MX; x++)	LCD_DrawChar(data[out_buf][CHARS_PER_LINE * 6 + x]);
		LCD_SetCursor(0, 7);	for (x = 0; x < 20; x++)	LCD_DrawChar(data[out_buf][CHARS_PER_LINE * 7 + x]);

		if ((!laser) && (!temps))
		{
			if ((data[out_buf][T_L_LINE] == 'C') ||			//COOL
				(data[out_buf][T_L_LINE + 5] == 'F') ||		//FLOW
				(data[out_buf][T_L_LINE + 10] == 'I') ||	//ILAZ
				(data[out_buf][T_L_LINE + 15] == 'C'))		//CUTT
			{
				if (data[out_buf][T_L_LINE] == 'C')			laser |= 1;
				if (data[out_buf][T_L_LINE + 5] == 'F')		laser |= 2;
				if (data[out_buf][T_L_LINE + 10] == 'I')	laser |= 4;
				if (data[out_buf][T_L_LINE + 15] == 'C')	laser |= 8;
			}
			else if (data[out_buf][T_L_LINE + 2] == '1')
				temps = 3;
			else
				temps = 1;
		}
	}
	CS_LCD_set;
}
//----------------------------------------------------------------------------
void Buzzer()
{	//set new freq
	uint32_t divider = 256 * freq[buzcntcur];
	uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / divider);
	Timer_P->PSC = PrescalerValue;
	Timer_P->BUZZER_CCR = 127;
	Timer_D->CR1 |= TIM_CR1_CEN;
}
//----------------------------------------------------------------------------
void Draw_Icons()
{
#define	LOGO_OFFSET		((CHARS_PER_LINE - 12) / 2)

#ifndef ONLY_MARLIN
	char border[] = {196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 0};
#endif

	pics = data[out_buf][FB_SIZE - 2];
	pics &= PIC_MASK;

	if (pics & PIC_LOGO)
	{	//Text Logo
		LCD_Set_TextColor(Yellow, Blue);
#ifndef ONLY_MARLIN
		if (protocol == Smoothie)
		{
			LCD_DrawChar_XY(LOGO_OFFSET, 4, TLC);	LCD_PutStrig(&border[0]);	LCD_DrawChar(TRC);
			LCD_DrawChar_XY(LOGO_OFFSET, 5, VL);	LCD_PutStrig("   Open   ");	LCD_DrawChar(VL);
			LCD_DrawChar_XY(LOGO_OFFSET, 6, VL);	LCD_PutStrig(" Hardware ");	LCD_DrawChar(VL);
			LCD_DrawChar_XY(LOGO_OFFSET, 7, BLC);	LCD_PutStrig(&border[0]);	LCD_DrawChar(BRC);
		}
		else
#endif
		{
			uint16_t i = 0;
			for (uint8_t y = 0; y < 5; y++)
			{
				LCD_SetCursor(0, y);
				for (uint8_t x = 0; x < CHARS_PER_LINE; x++)
				{
					if (data[out_buf][i] == '-')	LCD_DrawChar(196);
					else							LCD_DrawChar(data[out_buf][i]);
					i++;
				}
			}
		}
		CS_LCD_set;
		LCD_Set_TextColor(White, BackColor);
		if (init)
		{
			duration[buzcnt] = 200;	freq[buzcnt++] = LOGO_FREQ1;
			duration[buzcnt] = 200;	freq[buzcnt++] = LOGO_FREQ2;
			duration[buzcnt] = 200;	freq[buzcnt++] = LOGO_FREQ3;
			Buzzer();
			init = 0;
		}
		return;
	}

	if ((!temps) && (!laser))	return;

#ifdef USE_LASER
	if (laser)
	{
		if (laser & 1)
		{
			if (pics & PIC_HE1) LCD_Draw_Picture (pic_COOL, pic_Ymin, &cool_48x48[0]);
			else				LCD_Draw_Picture (pic_COOL, pic_Ymin, &cool_off_48x48[0]);
		}
		if (laser & 2)
		{
			if (pics & PIC_FAN)	LCD_Draw_Picture (pic_FLOW, pic_Ymin, &flow_48x48[0]);
			else				LCD_Draw_Picture (pic_FLOW, pic_Ymin, &flow_off_48x48[0]);
		}
		if (laser & 4)
		{
			if (pics & PIC_BED)	LCD_Draw_Picture (pic_ILAZ, pic_Ymin, &amp_48x48[0]);
			else				LCD_Draw_Picture (pic_ILAZ, pic_Ymin, &amp_off_48x48[0]);
		}
		if (laser & 8)
		{
			if (pics & PIC_HOT)	LCD_Draw_Picture (pic_CUTT, pic_Ymin, &cutt_48x48[0]);
			else				LCD_Draw_Picture (pic_CUTT, pic_Ymin, &cutt_off_48x48[0]);
		}
	}
#endif
#ifdef USE_3DPRINTER
	if (temps == 1)
	{
		if (pics & PIC_HE1) LCD_Draw_Picture (pic1_Xmin1, pic_Ymin, &extrude_48x48[0]);
		else				LCD_Draw_Picture (pic1_Xmin1, pic_Ymin, &extrude_off_48x48[0]);

		if (pics & PIC_BED)	LCD_Draw_Picture (pic2_Xmin1, pic_Ymin, &bed_48x48[0]);
		else
		{
#ifdef ONLY_MARLIN
			if (data[out_buf][T_L_LINE + B1O] == 'B')	//bed present in off state
#else
			if (((protocol == Smoothie) && (datat[TTO + B1O] == 'B')) ||
				((protocol != Smoothie) && (data[out_buf][T_L_LINE + B1O] == 'B')))	//bed present in off state
#endif
				LCD_Draw_Picture (pic2_Xmin1, pic_Ymin, &bed_off_48x48[0]);
			else
				LCD_Clear_Picture(pic2_Xmin1, pic_Ymin);
		}

		if (pics & PIC_FAN)	LCD_Draw_Picture (pic3_Xmin1, pic_Ymin, &fan_48x48[0]);
		else
		{
#ifdef ONLY_MARLIN
			if (data[out_buf][CHARS_PER_LINE * 7 + F1O + 2] >= '0')	//fan present in off state
#else
			if (((protocol == Smoothie) && ((datat[TSO + F1O + 1] >= '0') || (datat[TSO + F1O + 2] >= '0'))) ||
				((protocol != Smoothie) && (data[out_buf][CHARS_PER_LINE * 7 + F1O + 2] >= '0')))
#endif
				LCD_Draw_Picture (pic3_Xmin1, pic_Ymin, &fan_off_48x48[0]);
			else
				LCD_Clear_Picture(pic3_Xmin1, pic_Ymin);
		}

#ifndef WITHOUT_HEAT_ICO
#ifndef ONLY_MARLIN
		if (protocol != Smoothie)
#endif
		{
			if (pics & PIC_HOT)	LCD_Draw_Picture (pic4_Xmin1, pic_Ymin, &heat_48x48[0]);
			else				LCD_Clear_Picture(pic4_Xmin1, pic_Ymin);
		}
#endif
	}
	else if (temps == 3)
	{
		if (pics & PIC_HE1)	LCD_Draw_Picture (pic1_Xmin, pic_Ymin, &extrude_48x48[0]);
		else				LCD_Draw_Picture (pic1_Xmin, pic_Ymin, &extrude_off_48x48[0]);

		if (pics & PIC_HE2)	LCD_Draw_Picture (pic2_Xmin, pic_Ymin, &extrude_48x48[0]);
		else				LCD_Draw_Picture (pic2_Xmin, pic_Ymin, &extrude_off_48x48[0]);

		if (pics & PIC_HE3)	LCD_Draw_Picture (pic3_Xmin, pic_Ymin, &extrude_48x48[0]);
		else
		{
#ifdef ONLY_MARLIN
			if (data[out_buf][T_L_LINE + HE3O] == 'H')
#else
			if (((protocol == Smoothie) && (datat[TTO + HE3O] == 'H')) ||
				((protocol != Smoothie) && (data[out_buf][T_L_LINE + HE3O] == 'H')))
#endif
				LCD_Draw_Picture (pic3_Xmin, pic_Ymin, &extrude_off_48x48[0]);
			else
				LCD_Clear_Picture(pic3_Xmin, pic_Ymin);
		}

		if (pics & PIC_BED)	LCD_Draw_Picture (pic4_Xmin, pic_Ymin, &bed_48x48[0]);
		else
		{
#ifdef ONLY_MARLIN
			if (data[out_buf][T_L_LINE + B3O] == 'B')
#else
			if (((protocol == Smoothie) && (datat[TTO + B3O] == 'B')) ||
				((protocol != Smoothie) && (data[out_buf][T_L_LINE + B3O] == 'B')))
#endif
				LCD_Draw_Picture (pic4_Xmin, pic_Ymin, &bed_off_48x48[0]);
			else
				LCD_Clear_Picture(pic4_Xmin, pic_Ymin);
		}

		if (pics & PIC_FAN)	LCD_Draw_Picture (pic5_Xmin, pic_Ymin, &fan_48x48[0]);
		else
		{
#ifdef ONLY_MARLIN
			if (data[out_buf][CHARS_PER_LINE * 7 + F3O + 2] >= '0')
#else
			if (((protocol == Smoothie) && ((datat[TSO + F3O + 1] >= '0') || (datat[TSO + F3O + 2] >= '0'))) ||
				((protocol != Smoothie) && (data[out_buf][CHARS_PER_LINE * 7 + F3O + 2] >= '0')))
#endif
				LCD_Draw_Picture (pic5_Xmin, pic_Ymin, &fan_off_48x48[0]);
			else
				LCD_Clear_Picture(pic5_Xmin, pic_Ymin);
		}

#ifndef WITHOUT_HEAT_ICO
#ifndef ONLY_MARLIN
		if (protocol != Smoothie)
#endif
		{
			if (pics & PIC_HOT)	LCD_Draw_Picture (pic6_Xmin, pic6_Ymin, &heat_48x48[0]);
			else				LCD_Clear_Picture(pic6_Xmin, pic6_Ymin);
		}
#endif
	}
#endif
}
//----------------------------------------------------------------------------
void Draw_Progress_Bar(uint8_t y, uint8_t percent)
{
	uint8_t i, ymin, ymax;

	const uint16_t pb_colors[] = {0xf816, 0xf80a, 0xf800, 0xfa80, 0xfc00, 0xfda0, 0xef60, 0x57e0, 0x07f6, 0x07ff, 0x05bf};

#define XMIN	(LCDXMAX - 306) / 2	//center progress bar to text line
#define XMAX	XMIN + 306			//100% * 3 + 6 for frame

	ymin = y * CHAR_HEIGTH + 1;
	ymax = ymin + CHAR_HEIGTH - 2;

	if ((!progress_cleared) && (y == 2))
	{
		//clear Progress Bar line
		LCD_FillRect(0, CHAR_HEIGTH * 2, LCDXMAX, CHAR_HEIGTH * 3, BackColor);
		// clear icons positions
		LCD_FillRect(CHAR_WIDTH * 16, CHAR_HEIGTH * 5, LCDXMAX - 1, CHAR_HEIGTH * 8, BackColor);
		LCD_FillRect(0, CHAR_HEIGTH * 8, LCDXMAX, LCDYMAX - 1, BackColor);
		progress_cleared = 1;
	}

	//draw progress bar frame
	LCD_FillRect(XMIN, ymin, XMIN, ymax, PROGRESS_COLOR);			// left |
	LCD_FillRect(XMIN + 1, ymin, XMAX, ymin, PROGRESS_COLOR);		// top -
	LCD_FillRect(XMIN + 1, ymax, XMAX, ymax, PROGRESS_COLOR);		// bottom -
	LCD_FillRect(XMAX, ymin + 1, XMAX, ymax - 1, PROGRESS_COLOR);	// right |

	if (percent) //draw progress bar
	{
		i = percent / 10;
		LCD_FillRect(XMIN + 3, ymin + 2, XMIN + 3 + percent * 3, ymax - 2, pb_colors[i]);
		if (percent < 100)
			LCD_FillRect(XMIN + 4 + percent * 3, ymin + 2, XMAX - 3, ymax - 2, Black);
	}
	else
		LCD_FillRect(XMIN + 3, ymin + 2, XMAX - 3, ymax - 2, Black);
}
//----------------------------------------------------------------------------
void Print_Line(uint8_t row)
{
	uint16_t i;
	uint8_t x, marker;
	uint8_t *line;

	marker = 1;

	i = row * CHARS_PER_LINE;

	line = &data[out_buf][i];
#ifdef HW_VER_3
	if (screen_mode == EDIT_SCREEN)
	{
		if      (row == (MIDDLE_Y + 2))	line = &line_3[0];
		else if (row == (MIDDLE_Y + 4))	line = &line_1[0];
	}
#endif

	if (*line == '%')
	{
#ifdef HW_VER_3
		if (row == (MIDDLE_Y + 1))
		{
			screen_mode = EDIT_SCREEN;
			lines[MIDDLE_Y + 2] = lines[MIDDLE_Y + 4] = 1;
			row = MIDDLE_Y - 1;	//move progress bar up to separate from buttons
			clr_lines[MIDDLE_Y - 1] = clr_lines[MIDDLE_Y + 2] = clr_lines[MIDDLE_Y + 4] = 0;
		}
#endif
		Draw_Progress_Bar(row, *(line + 1));
		return;
	}
	//change colors for different lines
	else if ((*line == '>') || (*line == 0x03))
	{
		USE_UBL = 0;
		LCD_Set_TextColor(CURSOR_TEXT_COLOR, CURSOR_BACK_COLOR);	//menu cursor
#ifdef HW_VER_3
		screen_mode = MENU_SCREEN;
		cur_line = row;
#endif
	}
	else if (*line == '#')
	{
		LCD_Set_TextColor(EDIT_TEXT_COLOR, EDIT_BACK_COLOR); //edit line
#ifndef HW_VER_3
		if (USE_UBL && row != 9)	return;
#else
		if (USE_UBL || row == 9)
		{
			screen_mode = EDIT_UBL;
			if (row != 9)	return;
		}
		else
		{
			if ((*(line + 1) == '[') || (*(line + CHARS_PER_LINE - 2) == ']'))
				screen_mode = SELECT_SCREEN;
			else
			{
				screen_mode = EDIT_SCREEN;
				lines[MIDDLE_Y + 2] = lines[MIDDLE_Y + 4] = 1;
				clr_lines[MIDDLE_Y + 2] = clr_lines[MIDDLE_Y + 4] = 0;
			}
		}
#endif
	}
	else if (*line == '!')
		LCD_Set_TextColor(ERROR_TEXT_COLOR, ERROR_BACK_COLOR);		//errors line
	else
	{
		marker = 0;
		LCD_Set_TextColor(White, Black);	//default colors for text
	}

#ifdef HW_VER_3
	if (USE_UBL)
	{
		if (row == 9)
		{
			if (screen_mode == EDIT_UBL)
			{
				LCD_PutStrig_XY(12, 3, "Z:");
				LCD_SetCursor(14, 3);
				for (i = 14; i < 20; i++)	LCD_DrawChar(data[out_buf][(CHARS_PER_LINE * 10 - 7 - 14) + i]);
				LCD_Set_TextColor(CURSOR_TEXT_COLOR, CURSOR_BACK_COLOR);
				LCD_PutStrig_XY(13, 5, "[ Z+ ]");
				LCD_PutStrig_XY(13, 7, "[ Z- ]");
				LCD_Set_TextColor(White, Black);
				LCD_PutStrig_XY( 2, 8, "      ");
			}
			else
			{
				LCD_Set_TextColor(CURSOR_TEXT_COLOR, CURSOR_BACK_COLOR);
				LCD_PutStrig_XY(13, 5, "[NEXT]");
				LCD_PutStrig_XY(13, 7, "[PREV]");
				LCD_PutStrig_XY( 2, 8, "[EXIT]");
				LCD_Set_TextColor(White, Black);
			}
			CS_LCD_set;
			return;
		}
		else if (row == 8)
			row = 9;	//move text down to separate from buttons
	}
#endif

	LCD_SetCursor(0, row);

#ifndef ONLY_MARLIN
#ifdef LCD400x240
	if (protocol == Smoothie)
	{//shift text right for centering
		LCD_DrawChar(' ');	LCD_DrawChar(' ');
		if (marker)
		{
			LCD_DrawChar(' ');	line++;	x = 3;
		}
		else
			x = 2;
	}
	else
#endif
#endif
	{
		if (marker)
		{
			LCD_DrawChar(' ');	line++;	x = 1;
		}
		else
			x = 0;
	}

	while (x++ < CHARS_PER_LINE)
	{
#ifdef HW_VER_3
		if (screen_mode == EDIT_SCREEN)
		{
			if (*(line) == '[')
			{
				LCD_Set_TextColor(EDIT_TEXT_COLOR, EDIT_BACK_COLOR);
				LCD_DrawChar(0x12);
			}
			else if (*(line) == ']')
			{	
				LCD_DrawChar(0x12);
				LCD_Set_TextColor(White, Black);
			}
			else LCD_DrawChar(*(line));
			line++;
		}
		else
#endif
		LCD_DrawChar(*(line++));
	}
	CS_LCD_set;
}
//----------------------------------------------------------------------------
void Center_Status_Line()
{
	int8_t i, newpos;

#ifndef ONLY_MARLIN
	if (protocol == Smoothie)
	{
		i = 0;
		while ((i < CHARS_PER_LINE) && (data[out_buf][CHARS_PER_LINE * 3 + i] == ' '))	i++;
		if (i > 1)
		{
			newpos = i / 2 + 1;
			while (i < CHARS_PER_LINE)
				data[out_buf][CHARS_PER_LINE * 3 + newpos++] = data[out_buf][CHARS_PER_LINE * 3 + i++];
			while (newpos < CHARS_PER_LINE)
				data[out_buf][CHARS_PER_LINE * 3 + newpos++] = ' ';
		}
	}
	else
#endif
	{
		i = CHARS_PER_LINE - 1;
		while ((i > 0) && (data[out_buf][CHARS_PER_LINE * 3 + i] == ' '))	i--;
		{
			newpos = ((CHARS_PER_LINE - 1) - i) / 2 + i;
			while (i >= 0)
				data[out_buf][CHARS_PER_LINE * 3 + newpos--] = data[out_buf][CHARS_PER_LINE * 3 + i--];
			while (newpos >= 0)
				data[out_buf][CHARS_PER_LINE * 3 + newpos--] = ' ';
		}
	}
}
//----------------------------------------------------------------------------
void set_new_buz()
{
	duration[buzcnt] = data[in_buf][0] << 8;
	duration[buzcnt] += data[in_buf][1];
	freq[buzcnt] = data[in_buf][2] << 8;
	freq[buzcnt] += data[in_buf][3];
	freq[buzcnt] *= BUZ_FREQ_MP * 10;
	freq[buzcnt] /= 10;
	if (freq[buzcnt] > 2000)	freq[buzcnt] = 2000;
	if (duration[buzcnt] < 50)	duration[buzcnt] = 50;
	buzcnt++;
}
//----------------------------------------------------------------------------
void Init()
{
	uint16_t i;

	new_command = 0;
	protocol = data[in_buf][0];
	in_buf = 1;
	out_buf = 0;
	temps = 0;
	laser = 0;
	buzcntcur = buzcnt = 0;
	init = 1;
	data[out_buf][FB_SIZE - 1] = data[out_buf][FB_SIZE - 2] = 0;
	for (i = 0; i < 60; i++)	datat[i] = ' ';
	for (i = 0; i < (FB_SIZE - 2); i++)	data[in_buf][i] = ' ';
	data[in_buf][FB_SIZE - 2] = data[in_buf][FB_SIZE - 1] = 0;
	Clear_Screen();
	LCD_ClearScreen();
#ifdef HW_VER_3
	restoreSettings();
#endif
}
//----------------------------------------------------------------------------
void Out_Buffer()
{	//update time for screen is about 45..55ms for 320x240 resolution and 8 bit data bus
	uint16_t y;

	new_command = 0;

	if (new_buf == 1)
	{	//switch buffers only betwin buffer transfers
		new_buf = 0;
		out_buf ^= 1;
		in_buf ^= 1;
#ifndef ONLY_MARLIN
		if (protocol == Smoothie)
		{//for main screen
			if ((data[out_buf][CHARS_PER_LINE] == 'X') && (data[out_buf][CHARS_PER_LINE + 6] == 'Y'))	Move_Text();
		}
#endif
		if (data[out_buf][0] == 'X')
		{	//Main screen
#ifdef HW_VER_3
			screen_mode = MAIN_SCREEN;
#endif
			Center_Status_Line();
		}
	}

#ifndef ONLY_MARLIN
	if (protocol == Smoothie)
	{
		if ((data[out_buf][0] == 'X') && (data[out_buf][6] == 'Y'))
		{//main screen
			for (y = 0; y < 5; y++)		Print_Line(y);
			Print_Temps();
			Draw_Icons();
			if (!temps) return;	//first cycle
			Set_Leds();
		}
		else
		{//menu or start screen
			progress_cleared = 0;
			if (data[out_buf][FB_SIZE - 2] & PIC_LOGO)
			{
				for (y = 0; y < 4; y++)	Print_Line(y);
				Draw_Icons();
			}
			else
			{
				for (y = 0; y < TEXT_LINES; y++)
				{
					if (y == 1)	Check_for_edit_mode();
					Print_Line(y);
				}
			}
		}
	}
	else
#endif
	{	//Marlin
		if (data[out_buf][FB_SIZE - 2] & PIC_LOGO)
			Draw_Icons();
		else
		{
			LCD_Set_TextColor(White, Black);
			if (data[out_buf][12] == '(')
			{
#ifdef HW_VER_3
				screen_mode = MAIN_UBL;
#endif
				Draw_UBL_Screen();
			}
			else
			{
				for (y = 0; y < TEXT_LINES; y++)
				{
					if ((y == 5) && (data[out_buf][0] == 'X'))
					{//main screen
						USE_UBL = 0;
						Print_Temps();
						Draw_Icons();
						return;
					}
					else
						Print_Line(y);
				}
			}
		}
	}
}
//#################################################################################
// Interrupts
//#################################################################################
void Btn_IRQHandler(void)
{
	Timer_Btn->SR = 0;	//clear IRQ flag
	if (!new_command)
	{
#ifdef HW_VER_3
		Read_Touch();
		if (!buttons)
#endif
		buttons = Read_Buttons();
	}
}
//----------------------------------------------------------------------------
void Dur_IRQHandler(void)
{
	TIM_ClearITPendingBit(Timer_D, TIM_IT_Update);
	if (duration[buzcntcur])
		duration[buzcntcur]--;
	else
	{
		buzcntcur++;
		if (buzcntcur < buzcnt)
			Buzzer();
		else
		{
			Timer_D->CR1 &= ~TIM_CR1_CEN;	//stop timer2
			Timer_P->BUZZER_CCR = 0;		//buzzer off
			buzcnt = 0;
			buzcntcur = 0;
		}
	}
}
//----------------------------------------------------------------------------
#ifndef HW_VER_3
void ENC_IRQHandler(void)
{
	EXTI->PR = ENC_A;					//clear IRQ flag
	EXTI->IMR &= ~((uint32_t)ENC_A);	//disable this IRQ
	//start delay
	Timer_Del->CNT = 0;
	Timer_Del->CR1 |= TIM_CR1_CEN;	//delay on
}
#endif
//----------------------------------------------------------------------------
void Del_IRQHandler(void)
{
	Timer_Del->SR = 0;				//clear IRQ flag

#ifndef HW_VER_3
	Timer_Del->CR1 &= ~TIM_CR1_CEN;	//delay off
	if (!(ENC_PORT->IDR & ENC_A))
	{
#ifdef	INVERT_ENCODER_DIR
		if (ENC_PORT->IDR & ENC_B)	encdiff += 2;
		else						encdiff -= 2;
#else
		if (ENC_PORT->IDR & ENC_B)	encdiff -= 2;
		else						encdiff += 2;
#endif	// INVERT_ENCODER_DIR
	}
	EXTI->IMR |= ENC_A;	//enable EXTI IRQ for next front
#else
	if (++delay_cnt > 10)	delay_cnt = 10;
#endif
}
#ifdef USE_I2C
//----------------------------------------------------------------------------
// Only for Marlin
void I2C2_EV_IRQHandler(void)
{
	uint32_t event;
	uint8_t b;

	event = I2C_GetLastEvent(I2C);

	switch (event)
	{
		// Slave RECEIVER mode
	    case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:	//EV1
	    	return;
	    case I2C_EVENT_SLAVE_BYTE_RECEIVED:	//EV2
	    	b = I2C->DR;
	    	if (!toread)
	    	{// command
	    		cmd = b;	pos = -1;
		    	if ((b == INIT) || (b == BRIGHTNES) || (b == BUZZER) || (b == LCD_WRITE) || (b ==  LCD_PUT)) toread = 1; //read data for command
			}
	    	else
	    	{
	    		if ((cmd == LCD_WRITE) || (cmd == LCD_PUT))
	    		{
	    			if (pos == -1)
	    			{ //write to text buffer by one line
						if (cmd == LCD_WRITE)	{pos = 0;	data[in_buf][pos++] = b;}
						else					{pos = b * CHARS_PER_LINE;	lines[b] = 1;}
	    			}
	    			else
					{
						if (cmd == LCD_WRITE)	data[in_buf][pos++] = b;
						else					data[out_buf][pos++] = b;
					}
	    		}
	    		else
	    			data[in_buf][++pos] = b;	// read data for commands
			}
	    	return;
	    case I2C_EVENT_SLAVE_STOP_DETECTED:	//EV4 for write and read
	    	while ((I2C->SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR) { I2C->SR1; I2C->SR2; }	// ADDR-Flag clear
	    	while ((I2C->SR1 & I2C_SR1_STOPF) == I2C_SR1_STOPF) { I2C->SR1; I2C->CR1 |= 0x1; }	// STOPF Flag clear
			toread = 0;
			switch (cmd)
			{
				case BUZZER:	set_new_buz();	if(buzcnt == 1) Buzzer();	return;
				case BRIGHTNES:	Timer_P->BRIGHTNES_CCR = (uint16_t)data[in_buf][0];	return;
				case LCD_WRITE:	if (pos < FB_SIZE) {toread = 1;	return;}
								new_buf = 1;
				case INIT:
				case CLR_SCREEN:
				case LCD_PUT:	new_command = cmd;	return;
			}
	    // Slave TRANSMITTER mode
	    case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:	//EV1
	    	switch (cmd)
	    	{
	    		case READ_ENCODER:	I2C->DR = encdiff;	encdiff = 0;	return;
	    		case GET_LCD_ROW:	I2C->DR = TEXT_LINES;	return;
	    	}
	    case I2C_EVENT_SLAVE_BYTE_TRANSMITTED:	//EV3 - next data
	    	I2C->DR = buttons;	buttons = 0;
	    case I2C_EVENT_SLAVE_BYTE_TRANSMITTING:	//EV3
			return;
	}
}
//----------------------------------------------------------------------------
void I2C2_ER_IRQHandler(void)
{	//EV3_2
	I2C->SR1 &= 0x00FF;
}
#endif
#ifdef USE_SPI
//----------------------------------------------------------------------------
void SPI_IRQHandler(void)
{
	uint8_t b = SPI->DR;  // grab byte from SPI Data Register, reset Interrupt flag

	if (!toread)
	{// command
		switch(b)
		{
			case GET_SPI_DATA:	return;	//for reading data
			case READ_BUTTONS:	SPI->DR = buttons;	buttons = 0;	return;
			case READ_ENCODER:	SPI->DR = encdiff;	encdiff = 0;	return;
			case LCD_WRITE:		cmd = b;	toread = FB_SIZE;	pos = 0;	return;
			case LCD_PUT:		cmd = b;	toread = CHARS_PER_LINE;	pos = -1;	return;
			case BUZZER:		cmd = b;	toread = 4;	pos = 0;	return;
			case GET_LCD_ROW:	SPI->DR = TEXT_LINES;	return;
			case GET_LCD_COL:	SPI->DR = CHARS_PER_LINE;	return;
			case CLR_SCREEN:	new_command = b;	return;	//Clear_Screen();	return;
			case BRIGHTNES:
			case INIT:			cmd = b;	toread = 1;	pos = 0;	return;
		}
	}
	else
	{
		if (pos == -1)
		{
			pos = b * CHARS_PER_LINE;
			lines[b] = 1;
		}
		else
		{
			if (cmd == LCD_PUT)
				data[out_buf][pos++] = b;
			else
				data[in_buf][pos++] = b;
			toread--;
			if (!toread)
				switch (cmd)
				{
					case BUZZER:	set_new_buz();	if (buzcnt == 1)	Buzzer();	return;
					case BRIGHTNES:	Timer_P->BRIGHTNES_CCR = (uint16_t)data[in_buf][0];	return;
					case LCD_WRITE:	new_buf = 1;	//update all screen
					case INIT:
					case LCD_PUT:	new_command = cmd;
				}
		}
	}
}
#endif
