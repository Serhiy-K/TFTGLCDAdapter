#include "stm32f10x_i2c.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "LCD.h"
#include "Pictures.h"
#include "protocol.h"
#include "systick.h"


#define	TTO		0	// offset for text termo
#define	TSO		16	// offset for setted termo
#define	TMO		32	// offset for measured termo
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
#define F3O		17

#define pic_Ymin	LCDYMAX - 48
//for one extruder config
#define	pic1_Xmin1	CHAR_WIDTH * HEO	//HE
#define	pic2_Xmin1	CHAR_WIDTH * B1O	//BED
#define	pic3_Xmin1	CHAR_WIDTH * F1O	//FAN
#define pic4_Xmin1	CHAR_WIDTH * H1O	//HEAT
//for multy-extruders config
#define	pic1_Xmin	HE1O	//HE1
#define	pic2_Xmin	CHAR_WIDTH * HE2O	//HE2
#define	pic3_Xmin	CHAR_WIDTH * HE3O	//HE3
#define	pic4_Xmin	CHAR_WIDTH * B3O	//BED
#define	pic5_Xmin	CHAR_WIDTH * F3O	//FAN

#ifdef	LCD320x240
#define pic6_Xmin	pic5_Xmin	//HEAT
#define pic6_Ymin	pic_Ymin - (CHAR_HEIGTH * 3)
#endif
#ifdef LCD400x240
#define pic6_Xmin	CHAR_WIDTH * 21
#define pic6_Ymin	pic_Ymin
#endif

#define PROGRESS_COLOR		Yellow	//color for progress bar frame

#define CURSOR_TEXT_COLOR	Black
#define CURSOR_BACK_COLOR	White

#define EDIT_TEXT_COLOR		Yellow
#define EDIT_BACK_COLOR		Auqa

#define	ERROR_TEXT_COLOR	Yellow
#define ERROR_BACK_COLOR	Red

#define FB_SIZE	(CHARS_PER_LINE * TEXT_LINES + 2)	//text area + leds + pics

enum Commands {
	GET_SPI_DATA = 0,	// is a read results of previous command
	READ_BUTTONS,
	READ_ENCODER,
	LCD_WRITE,		// get data with output
	BUZZER,
	CONTRAST,
	// Other commands... 0xE0 thru 0xFF
	GET_LCD_ROW = 0xE0,
	GET_LCD_COL,
	INIT = 0xFE,	// Initialize
};

// data[FB_SIZE - 1] - leds
// data[FB_SIZE - 2] - pics
uint8_t	data[FB_SIZE];
uint8_t	datat[48] = {' '};

uint8_t	cmd = 0;
int16_t pos = -1;
uint8_t toread = 0;
uint8_t	new_command = 0;
uint8_t	cour_leds = 0;
uint8_t	temps = 1;
uint8_t	cour_row = 0;
uint16_t row_offset = 0;
uint8_t progress_cleared = 0;
uint8_t	protocol = Smoothie;
uint16_t freq = 0, duration = 0;
uint8_t pics;
uint16_t dot_pos_x = CHAR_WIDTH;
uint16_t dot_pos_y = 7 * CHAR_HEIGTH;
int8_t encdiff;
uint8_t c_p = 0;

//----------------------------------------------------------------------------
uint8_t New_cmd()	{ return new_command; }
//----------------------------------------------------------------------------
//For Smoothieware
//----------------------------------------------------------------------------
void Move_Text()
{
	uint8_t i, to;
	for (i = 0; i < 20; i++)
	{
		//scan first line for temperatures
		if (data[i] == ':')
		{
			if (data[i+1] == '0')
			{
				data[i+1] = ' ';	if (data[i+2] == '0')	data[i+2] = ' ';
			}
			if (data[i+5] == '0')
			{
				data[i+5] = ' ';	if (data[i+6] == '0')	data[i+6] = ' ';
			}
			switch (data[i-1])
			{
			case 'T':	//by default
			case 'E':	//extruder
			case 'H':	//hotend
				if (temps == 1)
				{	//when first step and hotend present
					to = TTO + HEO + 1;	datat[to++] = 'H';	datat[to] = 'E';
					to = TSO + HEO;	datat[to++] = data[i+1];	datat[to++] = data[i+2];	datat[to] = data[i+3];
					to = TMO + HEO;	datat[to++] = data[i+5];	datat[to++] = data[i+6];	datat[to] = data[i+7];
				}
				else
				{
					to =TTO + HE1O;	datat[to++] = 'H';	datat[to++] = 'E';	datat[to] = '1';
					to =TSO + HE1O;	datat[to++] = data[i+1];	datat[to++] = data[i+2];	datat[to++] = data[i+3];
					to =TMO + HE1O;	datat[to++] = data[i+5];	datat[to++] = data[i+6];	datat[to++] = data[i+7];
				}
				break;
			case 'B':
				if (temps == 1)
				{	//when first step and one hotend
					to = TTO + B1O;	datat[to++] = 'B';	datat[to++] = 'E';	datat[to] = 'D';
					to = TSO + B1O;	datat[to++] = data[i+1];	datat[to++] = data[i+2];	datat[to] = data[i+3];
					to = TMO + B1O;	datat[to++] = data[i+5];	datat[to++] = data[i+6];	datat[to] = data[i+7];
				}
				else
				{
					to = TTO + B3O;	datat[to++] = 'B';	datat[to++] = 'E';	datat[to++] = 'D';
					to = TSO + B3O;	datat[to++] = data[i+1];	datat[to++] = data[i+2];	datat[to] = data[i+3];
					to = TMO + B3O;	datat[to++] = data[i+5];	datat[to++] = data[i+6];	datat[to] = data[i+7];
				}
				break;
			case '2':
				if (temps == 1)
				{ // when first step
					temps = 3;
					uint8_t j;
					//clean previous text data
					for (j = TTO; j < TTO + F1O; j++ ) datat[j] = ' ';
					for (j = TSO; j < TSO + F1O; j++ ) datat[j] = ' ';
					for (j = TMO; j < TMO + F1O; j++ ) datat[j] = ' ';
					//clean previous icons
					LCD_ClearArea(0, pic_Ymin, LCDXMAX - 1, LCDYMAX - 1, BackColor);
				}
				to = TTO + HE2O;	datat[to++] = 'H';	datat[to++] = 'E';	datat[to] = '2';
				to = TSO + HE2O;	datat[to++] = data[i+1];	datat[to++] = data[i+2];	datat[to] = data[i+3];
				to = TMO + HE2O;	datat[to++] = data[i+5];	datat[to++] = data[i+6];	datat[to] = data[i+7];
				break;
			case '3':
				to = TTO + HE3O;	datat[to++] = 'H';	datat[to++] = 'E';	datat[to] = '3';
				to = TSO + HE3O;	datat[to++] = data[i+1];	datat[to++] = data[i+2];	datat[to] = data[i+3];
				to = TMO + HE3O;	datat[to++] = data[i+5];	datat[to++] = data[i+6];	datat[to] = data[i+7];
				break;
			}
		}
	}
	//move up 2 and 3 lines
	for (i = 0; i < CHARS_PER_LINE * 2; i++)	{data[i] = data[i + CHARS_PER_LINE];}
}
//----------------------------------------------------------------------------
uint8_t  Get_Progress()
{
	uint8_t percent;
	percent = data[CHARS_PER_LINE + 18] - '0';	//fixed position
	if (data[CHARS_PER_LINE + 17] != ' ')	percent += (data[CHARS_PER_LINE + 17] - '0') * 10;
	if (data[CHARS_PER_LINE + 16] == '1')	percent = 100;
	return percent;
}
//----------------------------------------------------------------------------
void Check_for_edit_mode()
{	//on edit param screen line 2 is free and line 3 has text
	uint8_t i;
	//check line 2
	for (i = 0; i < CHARS_PER_LINE; i++)
	{
		if (data[CHARS_PER_LINE + i] != ' ') return;	//line 2  has text
	}
	//check line 3
	for (i = CHARS_PER_LINE * 2; i < CHARS_PER_LINE * 3; i++)
	{
		if (data[i] != ' ')
		{	//line 3 has text
			for (i = CHARS_PER_LINE * 3 - 1; i > CHARS_PER_LINE * 2; i--)	data[i] = data[i - 1];	//shift text right
			data[CHARS_PER_LINE * 2] = '#';	//put marker for new colors
			return;
		}
	}
}
//----------------------------------------------------------------------------
void SetLeds()
{
	uint8_t leds;

	leds = data[FB_SIZE - 1];
	leds &= LED_MASK;

	if (leds & LED_HOT)
	{
		if (temps == 1)	LCD_Draw_Picture (pic4_Xmin1, pic_Ymin, &heat_48x48[0]);
#if defined(LCD400x240) || !defined(WITHOUT_HEAT_ICO)
		else			LCD_Draw_Picture (pic6_Xmin, pic6_Ymin, &heat_48x48[0]);
#endif
	}
	else
	{
		if (temps == 1)	LCD_Clear_Picture(pic4_Xmin1, pic_Ymin);
#if defined(LCD400x240) || !defined(WITHOUT_HEAT_ICO)
		else			LCD_Clear_Picture(pic6_Xmin, pic6_Ymin);
#endif
	}
}
//----------------------------------------------------------------------------
// For Marlin
//----------------------------------------------------------------------------
uint8_t grid_points_x, grid_points_y;
void UBL_Draw_Frame()
{
	uint8_t i;

	grid_points_x = data[1] >> 4;
	grid_points_y = data[1] & 0x0f;

	//top line
	for (i = 1; i < 10; i++)	data[i] = 11;
	//top right corner
	data[i] = 9;
	//left line
	for (i = CHARS_PER_LINE; i < (CHARS_PER_LINE * 7); i += CHARS_PER_LINE) data[i] = 8;
	//right line
	for (i = CHARS_PER_LINE + 10; i < (CHARS_PER_LINE * 7 + 10); i += CHARS_PER_LINE) data[i] = 8;
	//bottom left corner
	data[CHARS_PER_LINE * 7] = '+';
	//bottom line
	for (i = CHARS_PER_LINE * 7 + 1; i < (CHARS_PER_LINE * 7 + 10); i++) data[i] = 11;
	//bottom right corner
	data[CHARS_PER_LINE * 7 + 10] = 217;
}
//----------------------------------------------------------------------------
void UBL_Draw_Dot()
{
	uint8_t i;
	uint8_t point_x, point_y;	//point_y - inverted
	uint8_t step_x, step_y;

	step_x = CHAR_WIDTH * 9 / (grid_points_x - 1);
	step_y = CHAR_HEIGTH * 6 / (grid_points_y - 1);

	i = 0;
	while (data[++i] != ',');	//scan first line for point position

	point_x = data[i - 1] - '0';
	if (data[i - 2] != '(')	//point_x >= 10
		point_x += (data[i - 2] - '0') * 10;

	if (data[i + 2] != ')') {	//point_y >= 10
		point_y = (data[i + 1] - '0') * 10;
		point_y += data[i + 2] - '0';
	}
	else {
		point_y = data[i + 1] - '0';
	}

	dot_pos_x = CHAR_WIDTH + point_x * step_x;
	dot_pos_y = 7 * CHAR_HEIGTH - point_y * step_y;

	LCD_ClearArea(dot_pos_x - 3, dot_pos_y - 3, dot_pos_x + 3, dot_pos_y + 3, White);
}
//----------------------------------------------------------------------------
// For Smoothieware and Marlin
//----------------------------------------------------------------------------
void Print_Temps()
{
	#if defined(LCD320x240) && !defined(WITHOUT_HEAT_ICO)
		#define MX	16
	#else
		#define MX	20
	#endif
	uint8_t i, x, y;
	i = 0;
	//main screen
	if (protocol == Smoothie)
	{
		for (y = 5; y < 8; y++)
		{
			LCD_SetCursor(0, y);
			for (x = 0; x < 16; x++)	LCD_DrawChar(datat[i++]);
		}
	}
	else if (protocol == Marlin)
	{
		LCD_SetCursor(0, 5);	for (x = 0; x < MX; x++)	LCD_DrawChar(data[CHARS_PER_LINE * 5 + x]);
		LCD_SetCursor(0, 6);	for (x = 0; x < MX; x++)	LCD_DrawChar(data[CHARS_PER_LINE * 6 + x]);
		LCD_SetCursor(0, 7);	for (x = 0; x < 20; x++)	LCD_DrawChar(data[CHARS_PER_LINE * 7 + x]);
		if (data[CHARS_PER_LINE * 5 + 2] == '1') temps = 3;
	}
	CS_LCD_set;
}
//----------------------------------------------------------------------------
uint8_t Get_Buttons()
{
	int8_t b;

	b = 0;
	if (protocol == Smoothie)
	{
		if ((ENC_PORT->IDR & ENC_BUT) == 0)			b |= BUTTON_SELECT;
		if ((BTN_PORTA->IDR & BUTTON_PIN1) == 0)	b |= BUTTON_PAUSE;
		if ((BTN_PORTA->IDR & BUTTON_PIN2) == 0)	b |= BUTTON_RIGHT;
		if ((BTN_PORTA->IDR & BUTTON_PIN3) == 0)	b |= BUTTON_LEFT;
		if ((BTN_PORTB->IDR & BUTTON_PIN4) == 0)	b |= BUTTON_AUX1;
		if ((BTN_PORTB->IDR & BUTTON_PIN5) == 0)	b |= BUTTON_AUX2;
	}
	else if (protocol == Marlin)
	{
		if ((ENC_PORT->IDR & ENC_BUT) == 0)			b |= EN_C;
		if ((BTN_PORTA->IDR & BUTTON_PIN1) == 0)	b |= EN_D;
		if ((BTN_PORTA->IDR & BUTTON_PIN2) == 0)	b |= KILL;
	}
	return b;
}
//----------------------------------------------------------------------------
void buzzer()
{
	if (duration)
	{
		if (duration < 100)	duration = 100;
		TIM3->BUZZER_CCR = 127;
		TIM2->CR1 |= TIM_CR1_CEN;
	}
}
//----------------------------------------------------------------------------
void DrawIcons()
{
#define	LOGO_OFFSET		((CHARS_PER_LINE - 12) / 2)

	char border[] = {196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 0};

	pics = data[FB_SIZE - 2];
	pics &= PIC_MASK;

	if (pics & PIC_LOGO)
	{	//Text Logo
		LCD_Set_TextColor(Yellow, Blue);
		if (protocol == Smoothie)
		{
			LCD_DrawChar_XY(LOGO_OFFSET, 4, 218);	LCD_PutStrig(&border[0]);	LCD_DrawChar(191);	CS_LCD_set;
			LCD_DrawChar_XY(LOGO_OFFSET, 5, 179);	LCD_PutStrig("   Open   ");	LCD_DrawChar(179);	CS_LCD_set;
			LCD_DrawChar_XY(LOGO_OFFSET, 6, 179);	LCD_PutStrig(" Hardware ");	LCD_DrawChar(179);	CS_LCD_set;
			LCD_DrawChar_XY(LOGO_OFFSET, 7, 192);	LCD_PutStrig(&border[0]);	LCD_DrawChar(217);	CS_LCD_set;
		}
		else if (protocol == Marlin)
		{
			uint8_t i = 0;
			for (uint8_t y = 0; y < 5; y++)
			{
				LCD_SetCursor(0, y);
				for (uint8_t x = 0; x < CHARS_PER_LINE; x++)
				{
					if (data[i] == '-')
					{
						LCD_DrawChar(11);
						i++;
					}
					else
						LCD_DrawChar(data[i++]);
				}
			}
			CS_LCD_set;
		}
		LCD_Set_TextColor(White, BackColor);
		duration = 500;
		buzzer();
		return;
	}

	if (temps == 1)
	{
		if (pics & PIC_HE1) LCD_Draw_Picture (pic1_Xmin1, pic_Ymin, &extrude_48x48[0]);
		else				LCD_Clear_Picture(pic1_Xmin1, pic_Ymin);

		if (pics & PIC_BED)	LCD_Draw_Picture (pic2_Xmin1, pic_Ymin, &bed_48x48[0]);
		else				LCD_Clear_Picture(pic2_Xmin1, pic_Ymin);

		if (pics & PIC_FAN)	LCD_Draw_Picture (pic3_Xmin1, pic_Ymin, &fan_48x48[0]);
		else				LCD_Clear_Picture(pic3_Xmin1, pic_Ymin);

		if (protocol != Smoothie)
		{
			if (pics & PIC_HOT)	LCD_Draw_Picture (pic4_Xmin1, pic_Ymin, &heat_48x48[0]);
			else				LCD_Clear_Picture(pic4_Xmin1, pic_Ymin);
		}
	}
	else
	{
		if (pics & PIC_HE1)	LCD_Draw_Picture (pic1_Xmin, pic_Ymin, &extrude1_48x48[0]);
		else				LCD_Clear_Picture(pic1_Xmin, pic_Ymin);

		if (pics & PIC_HE2)	LCD_Draw_Picture (pic2_Xmin, pic_Ymin, &extrude2_48x48[0]);
		else				LCD_Clear_Picture(pic2_Xmin, pic_Ymin);

		if (pics & PIC_HE3)	LCD_Draw_Picture (pic3_Xmin, pic_Ymin, &extrude3_48x48[0]);
		else				LCD_Clear_Picture(pic3_Xmin, pic_Ymin);

		if (pics & PIC_BED)	LCD_Draw_Picture (pic4_Xmin, pic_Ymin, &bed_48x48[0]);
		else				LCD_Clear_Picture(pic4_Xmin, pic_Ymin);

		if (pics & PIC_FAN)	LCD_Draw_Picture (pic5_Xmin, pic_Ymin, &fan_48x48[0]);
		else				LCD_Clear_Picture(pic5_Xmin, pic_Ymin);

#if defined(LCD400x240) || !defined(WITHOUT_HEAT_ICO)
		if (protocol != Smoothie)
		{
			if (pics & PIC_HOT)	LCD_Draw_Picture (pic6_Xmin, pic6_Ymin, &heat_48x48[0]);
			else				LCD_Clear_Picture(pic6_Xmin, pic6_Ymin);
		}
#endif
	}
}
//----------------------------------------------------------------------------
void Draw_Progress_Bar(uint8_t y, uint8_t percent)
{
	uint8_t i, ymin, ymax;

	const uint16_t pb_colors[] = {0xfc00, 0xfcc0, 0xfd80, 0xfe40, 0xff20, 0xffe0, 0xcfe0, 0x97e0, 0x67e0, 0x37e0, 0x07e0};

#define XMIN	(LCDXMAX - 306) / 2	//center progress bar to text line
#define XMAX	XMIN + 306			//100% * 3 + 6 for frame

	ymin = y * CHAR_HEIGTH + 1;
	ymax = ymin + CHAR_HEIGTH - 2;

	if (progress_cleared == 0)
	{ // after return from long menu
		//clear Progress Bar line
		LCD_SetCursor(0, y);	for (i = 0; i < CHARS_PER_LINE; i++)	LCD_DrawChar(' ');
		// clear icons positions
		LCD_SetCursor(16, 5);	for (i = 16; i < CHARS_PER_LINE; i++)	LCD_DrawChar(' ');
		LCD_SetCursor(16, 6);	for (i = 16; i < CHARS_PER_LINE; i++)	LCD_DrawChar(' ');
		LCD_SetCursor(16, 7);	for (i = 16; i < CHARS_PER_LINE; i++)	LCD_DrawChar(' ');
		LCD_SetCursor(0, 8);	for (i = 0; i < CHARS_PER_LINE; i++)	LCD_DrawChar(' ');
		LCD_SetCursor(0, 9);	for (i = 0; i < CHARS_PER_LINE; i++)	LCD_DrawChar(' ');
		progress_cleared = 1;
	}

	//draw progress bar frame
	LCD_ClearArea(XMIN, ymin, XMIN, ymax, PROGRESS_COLOR);			// left |
	LCD_ClearArea(XMIN + 1, ymin, XMAX, ymin, PROGRESS_COLOR);		// top -
	LCD_ClearArea(XMIN + 1, ymax, XMAX, ymax, PROGRESS_COLOR);		// bot -
	LCD_ClearArea(XMAX, ymin + 1, XMAX, ymax - 1, PROGRESS_COLOR);	// right |

	if (c_p != percent)
	{//change progress bar
		if (percent > c_p) //draw progress bar
		{
			i = percent / 10;
			LCD_ClearArea(XMIN + 3, ymin + 2, XMIN + 3 + percent * 3, ymax - 2, pb_colors[i]);
		}
		else
			LCD_ClearArea(XMIN + 3, ymin + 2, XMAX - 3, ymax - 2, Black);
		c_p = percent;
	}
}
//----------------------------------------------------------------------------
void Print_Line(uint8_t row)
{
	uint16_t i = row * CHARS_PER_LINE;
	uint8_t x;

	//change colors for different lines
	if ((data[i] == '>') || (data[i] == 0x03))
	{//menu cursor
		progress_cleared = 0;
		if (data[i] == '>')	data[i] = ' ';
		LCD_Set_TextColor(CURSOR_TEXT_COLOR, CURSOR_BACK_COLOR);
	}
	else if (data[i] == '#')
	{//edit line
		progress_cleared = 0;
		data[i] = ' ';
		LCD_Set_TextColor(EDIT_TEXT_COLOR, EDIT_BACK_COLOR);
	}
	else if (data[i] == '!')
	{//errors line
		data[i] = ' ';
		LCD_Set_TextColor(ERROR_TEXT_COLOR, ERROR_BACK_COLOR);
	}
	else
		LCD_Set_TextColor(White, Black);	//default colors for text

	LCD_SetCursor(0, row);
#ifdef LCD400x240
	if (protocol == Smoothie)
	{//shift text right for centering
		LCD_DrawChar(' ');	LCD_DrawChar(' ');
		for (x = 2; x < CHARS_PER_LINE; x++)	LCD_DrawChar(data[i++]);
	}
	else
#else
	for (x = 0; x < CHARS_PER_LINE; x++)	LCD_DrawChar(data[i++]);
#endif
	CS_LCD_set;
}
//----------------------------------------------------------------------------
void handle_command()
{
	uint16_t y, i;

	//update time for screen is about 45ms for 320x240 resolution
	switch(cmd)
	{
		case INIT:
			protocol = data[0];
			progress_cleared = 0;
			temps = 1;
			if (protocol == Smoothie)
			{	//only on SPI bus
			    NVIC_DisableIRQ(I2C_IRQ);
			    NVIC_DisableIRQ(I2C_ERR_IRQ);
			    I2C_Cmd(I2C, DISABLE);
			}
			else if (protocol == Marlin)
			{
				NVIC_DisableIRQ(SPI_IRQ);
				SPI_Cmd(SPI, DISABLE);
			}
			LCD_FillScreen(BackColor);
			for (i = 0; i < (FB_SIZE - 2); i++)	data[i] = ' ';
			break;

		case LCD_WRITE:
			if (protocol == Smoothie)
			{
				if ((data[CHARS_PER_LINE] == 'X') && (data[CHARS_PER_LINE + 6] == 'Y'))
				{//main screen
					Move_Text();
					for (y = 0; y < 5; y++)
					{
						if (y == 2)	Draw_Progress_Bar(y, Get_Progress());
						else		Print_Line(y);
					}
					Print_Temps();
					DrawIcons();
					SetLeds();
				}
				else
				{
					for (y = 0; y < TEXT_LINES; y++)
					{
						if (y == 1)	Check_for_edit_mode();
						Print_Line(y);
					}
					if (data[FB_SIZE - 2] & PIC_LOGO)	DrawIcons();
				}
			}
			else
			{
			//print all screen or one line
				if (data[FB_SIZE - 2] & PIC_LOGO)
					DrawIcons();
				else
				{
					LCD_Set_TextColor(White, Black);
					if (pos == FB_SIZE)
					{ //print all screen
						if (data[0] == 218)
							UBL_Draw_Frame();
						for (y = 0; y < TEXT_LINES; y++)
						{
							row_offset = y * CHARS_PER_LINE;
							if (data[row_offset] == '%')
								Draw_Progress_Bar(y, data[row_offset + 1]);
							else if ((y == 5) && (data[0] == 'X'))
							{//main screen
								Print_Temps();
								DrawIcons();
								y = TEXT_LINES;
							}
							else
								Print_Line(y);
						}
						if (data[0] == 218)
							UBL_Draw_Dot();
					}
					else
					{//print one line
						if (data[row_offset] == '%')
							Draw_Progress_Bar(cour_row, data[row_offset + 1]);
						else
							Print_Line(cour_row);
					}
				}
			}
			break;	//LCD_WRITE

		case BUZZER:
			duration = data[0] << 8;
			duration += data[1];
			freq = data[2] << 8;
			freq += data[3];
			buzzer();
			break;

		case CONTRAST:
			i = data[0] << 8;
			TIM3->CONTRAST_CCR = i + data[1];
			break;
	}
	new_command = 0;
	toread = 0;
}
//#################################################################################
void TIM2_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	if (duration)
		duration--;
	else
	{
		TIM2->CR1 &= ~TIM_CR1_CEN;	//stop timer2
		TIM3->BUZZER_CCR = 0;		//buzzer off
	}
}
//----------------------------------------------------------------------------
void I2C2_EV_IRQHandler(void)
{
	uint32_t event;
	uint8_t b;

	event = I2C_GetLastEvent(I2C);

	switch (event)	{
		// Slave RECEIVER mode
	    case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:	//EV1
	    	break;
	    case I2C_EVENT_SLAVE_BYTE_RECEIVED:	//EV2
	    	b = I2C->DR;
	    	if (toread == 0)
	    	{// command
	    		cmd = b;	pos = -1;
		    	if ((b == INIT) || (b == CONTRAST) || (b == BUZZER) || (b == LCD_WRITE)) toread = 1; //read data for command
	    	}
	    	else
	    	{
	    		if (cmd == LCD_WRITE)
	    		{
	    			if (pos == -1)
	    			{ //write to text buffer by line
	    				cour_row = b;
	    				row_offset = b * CHARS_PER_LINE;
	    				pos = 0;
	    			}
	    			else
						data[row_offset + (uint16_t)pos++] = b;
	    		}
	    		else
	    			data[++pos] = b;	// read data for commands
			}
	    	break;
	    case I2C_EVENT_SLAVE_STOP_DETECTED:	//EV4 for write and read
	    	if (cmd)
	    		new_command = 1;
	    	while ((I2C->SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR) { I2C->SR1; I2C->SR2; }	// ADDR-Flag clear
	    	while ((I2C->SR1 & I2C_SR1_STOPF) == I2C_SR1_STOPF) { I2C->SR1; I2C->CR1 |= 0x1; }	// STOPF Flag clear
	    	break;
	    // Slave TRANSMITTER mode
	    case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:	//EV1
	    	switch (cmd)
	    	{
	    		case READ_BUTTONS:	I2C->DR = Get_Buttons();	break;
	    		case READ_ENCODER:	encdiff = (int8_t)TIM1->CNT;	TIM1->CNT = 0;
					#ifndef	INVERT_ENCODER_DIR
	    				I2C->DR = encdiff;
					#else
	    				I2C->DR = -encdiff;
					#endif
	    			break;
	    		case GET_LCD_ROW:	I2C->DR = TEXT_LINES;	break;
	    		case GET_LCD_COL:	I2C->DR = CHARS_PER_LINE;	break;
	    	}
	    	break;
	    case I2C_EVENT_SLAVE_BYTE_TRANSMITTING:	//EV3
	    	break;
	    case I2C_EVENT_SLAVE_BYTE_TRANSMITTED:	//EV3 - next data
	    	break;
	}
}
//----------------------------------------------------------------------------
void I2C2_ER_IRQHandler(void)
{	//EV3_2
	I2C->SR1 &= 0x00FF;
}
//----------------------------------------------------------------------------
void SPI2_IRQHandler(void)
{
	uint8_t b = SPI->DR;  // grab byte from SPI Data Register, reset Interrupt flag
	int16_t c;

	if (toread == 0)
	{// command
		switch(b)
		{
			case GET_SPI_DATA:	return;	//for reading data
			case READ_BUTTONS:	SPI->DR = Get_Buttons();	return;
			case READ_ENCODER:	c = TIM1->CNT;	TIM1->CNT = 0;
				#ifndef	INVERT_ENCODER_DIR
					SPI->DR = (int8_t)c;
				#else
					SPI->DR = -(int8_t)c;
				#endif
				return;
			case LCD_WRITE:		cmd = b;	toread = FB_SIZE;	pos = 0;	return;
			case BUZZER:		cmd = b;	toread = 4;	pos = 0;	return;
			case CONTRAST:		cmd = b;	toread = 1;	pos = 0;	return;
			case GET_LCD_ROW:	SPI->DR = TEXT_LINES;	return;
			case GET_LCD_COL:	SPI->DR = CHARS_PER_LINE;	return;
			case INIT:			cmd = b;	toread = 1;	pos = 0;	return;
		}
	}
	else
	{
		data[pos++] = b;
		toread--;
		if (toread == 0)	new_command = 1;
	}
}
