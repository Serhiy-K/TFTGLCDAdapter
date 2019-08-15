#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "LCD.h"
#include "Pictures.h"
#include "protocol.h"
#include "systick.h"

#define pic_Ymin	LCDYMAX - 48
#define pic_Ymax	pic_Ymin + 47

#ifdef ONE_HOTEND
#define	pic1_Xmin	16
#define	pic1_Xmax	pic1_Xmin + 47
#define	pic2_Xmin	96
#define	pic2_Xmax	pic2_Xmin + 47
#define	pic3_Xmin	176
#define	pic3_Xmax	pic3_Xmin + 47

#define led_Xmin	256
#define	led_Xmax	led_Xmin + 47
#define led_Ymin	pic_Ymin
#define led_Ymax	pic_Ymax
#else
#define	pic1_Xmin	0
#define	pic1_Xmax	pic1_Xmin + 47
#define	pic2_Xmin	64
#define	pic2_Xmax	pic2_Xmin + 47
#define	pic3_Xmin	128
#define	pic3_Xmax	pic3_Xmin + 47
#define	pic4_Xmin	208
#define	pic4_Xmax	pic4_Xmin + 47
#define	pic5_Xmin	LCDXMAX - 48
#define	pic5_Xmax	pic5_Xmin + 47

#define led_Xmin	pic5_Xmin
#define	led_Xmax	pic5_Xmin + 47
#define led_Ymin	LCDYMAX - 48 - 60
#define led_Ymax	pic_Ymin + 47
#endif

#define FB_SIZE	(CHARS_PER_LINE * TEXT_LINES + 2)	//text area + pics + leds

enum Commands
{
	GET_SPI_DATA = 0,	// is a read results of previous command
	READ_BUTTONS,
	READ_ENCODER,
	LCD_WRITE,
	BUZZER,
	CONTRAST,
	INIT = 0xFE,  // Initialize
};

static uint8_t	data[FB_SIZE];	// data[FB_SIZE - 2] - pics, data[FB_SIZE - 1] - leds
static uint8_t	cmd = 0;
static uint16_t pos = 0;
static uint16_t toread = 0;
static uint8_t	new_command = 0;
static uint8_t	buzzer_on = 0;
static uint8_t	cour_pics = 0;
static uint8_t	cour_leds = 0;

static uint8_t	datat[48] = {' '};

/***********************************************************
***********************************************************/
uint8_t New_cmd()
{
	return new_command;
}
/***********************************************************
 Remove leading zeros
***********************************************************/
void Rem_Lead_Zero(uint8_t i)
{
	if (data[i+1] == '0')
	{
		data[i+1] = ' ';
		if (data[i+2] == '0')	data[i+2] = ' ';
	}
	if (data[i+5] == '0')
	{
		data[i+5] = ' ';
		if (data[i+6] == '0')	data[i+6] = ' ';
	}
}
/***********************************************************
 Print temperatures for corresponding icons
***********************************************************/
void Get_Temps()
{
	for (uint8_t i = 0; i <20; i++)
	{	//scan first line for temperatures
		if (data[i] == ':')
		{	//temperature present
			switch (data[i-1])
			{
#ifdef ONE_HOTEND
			case 'T':	//temperature - default
			case 'E':	//extruder
			case 'H':	//hotend
				datat[2] = 'H';	datat[3] = 'E';
				Rem_Lead_Zero(i);
				//courrent temperature
				datat[17] = data[i+1];	datat[18] = data[i+2];	datat[19] = data[i+3];
				//new temperature
				datat[33] = data[i+5];	datat[34] = data[i+6];	datat[35] = data[i+7];
				break;
			case 'B':
				datat[6] = 'B';	datat[7] = 'E';	datat[8] = 'D';
				Rem_Lead_Zero(i);
				datat[22] = data[i+1];	datat[23] = data[i+2];	datat[24] = data[i+3];
				datat[38] = data[i+5];	datat[39] = data[i+6];	datat[40] = data[i+7];
#else
			case 'T':
			case 'E':
			case 'H':
				datat[0] = 'H';	datat[1] = 'E';	datat[2] = '1';
				Rem_Lead_Zero(i);
				datat[16] = data[i+1];	datat[17] = data[i+2];	datat[18] = data[i+3];
				datat[32] = data[i+5];	datat[33] = data[i+6];	datat[34] = data[i+7];
				break;
			case '2':
				datat[4] = 'H';	datat[5] = 'E';	datat[6] = '2';
				Rem_Lead_Zero(i);
				datat[20] = data[i+1];	datat[21] = data[i+2];	datat[22] = data[i+3];
				datat[36] = data[i+5];	datat[37] = data[i+6];	datat[38] = data[i+7];
				break;
			case '3':
				datat[8] = 'H';	datat[9] = 'E';	datat[10] = '3';
				Rem_Lead_Zero(i);
				datat[24] = data[i+1];	datat[25] = data[i+2];	datat[26] = data[i+3];
				datat[40] = data[i+5];	datat[41] = data[i+6];	datat[42] = data[i+7];
				break;
			case 'B':
				datat[13] = 'B';	datat[14] = 'E';	datat[15] = 'D';
				Rem_Lead_Zero(i);
				datat[29] = data[i+1];	datat[30] = data[i+2];	datat[31] = data[i+3];
				datat[45] = data[i+5];	datat[46] = data[i+6];	datat[47] = data[i+7];
#endif
				break;
			}
		}
	}
	for (uint8_t i = 0; i < 20; i++)	{data[i] = ' ';}
}
/***********************************************************
 Print temperatures
***********************************************************/
void Print_Temps()
{
	uint8_t i, x, y;
	i = 0;
	if (data[20] == 'X')
	{
		for (y = 5; y < 8; y++)
		{
			LCD_SetCursor(0, y);
			for (x = 0; x < 16; x++)
			{
				LCD_DrawChar(datat[i++]);
			}
			CS_LCD_clr;
		}
	}
}
/***********************************************************
 Get Buttons
***********************************************************/
uint8_t Get_Buttons()
{
	int8_t b;

	b = 0;
	if ((ENC_PORT->IDR & ENC_BUT) == 0)			b = BUTTON_SELECT;
	if ((BTN_PORTA->IDR & BUTTON_PIN1) == 0)	b |= BUTTON_PAUSE;	//set selected button
	return b;
}
/***********************************************************
 Set real leds or draw icon
***********************************************************/
void SetLeds()
{
	uint8_t leds;

	leds = data[FB_SIZE - 1];
	leds &= LED_MASK;

	if ((cour_leds & LED_HOT) != (leds & LED_HOT))
	{
		if (leds & LED_HOT)	LCD_Draw_Picture (led_Xmin, led_Ymin, &heat_48x48[0]);
		else				LCD_Clear_Picture(led_Xmin, led_Ymin);
	}
	cour_leds = leds;
}
/***********************************************************
 Draw icons for start logo, hotends, bed and cooler
***********************************************************/
void DrawIcons()
{
	const char border[] = {196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 0};

	uint8_t pics;

	pics = data[FB_SIZE - 2];
	pics &= PIC_MASK;

	LCD_PutStrig_XY(0, 4, "                    ");

	if (pics & PIC_LOGO)
	{	//Text Logo
		LCD_Set_TextColor(Yellow, Blue);
		//text in frame
		LCD_DrawChar_XY(4, 4, 192);	LCD_PutStrig(&border[0]);	LCD_DrawChar(217);	CS_LCD_clr;
		LCD_DrawChar_XY(4, 5, 179);	LCD_PutStrig("   Open   ");	LCD_DrawChar(179);	CS_LCD_clr;
		LCD_DrawChar_XY(4, 6, 179);	LCD_PutStrig(" Hardware ");	LCD_DrawChar(179);	CS_LCD_clr;
		LCD_DrawChar_XY(4, 7, 218);	LCD_PutStrig(&border[0]);	LCD_DrawChar(191);	CS_LCD_clr;
		LCD_Set_TextColor(White, BackColor);
		TIM3->BUZZER_CCR = 127;
		delay_ms(300);
		TIM3->BUZZER_CCR = 0;
	}
#ifdef ONE_HOTEND
	if ((cour_pics & PIC_HOT1) != (pics & PIC_HOT1))
	{
		if (pics & PIC_HOT1)LCD_Draw_Picture(pic1_Xmin, pic_Ymin, &extrude_48x48[0]);
		else				LCD_ClearArea   (pic1_Xmin, pic_Ymin, pic1_Xmax, pic_Ymax, BackColor);
	}
	if ((cour_pics & PIC_BED) != (pics & PIC_BED))
	{
		if (pics & PIC_BED)	LCD_Draw_Picture(pic2_Xmin, pic_Ymin, &bed_48x48[0]);
		else				LCD_ClearArea   (pic2_Xmin, pic_Ymin, pic2_Xmax, pic_Ymax, BackColor);
	}
	if ((cour_pics & PIC_FAN) != (pics & PIC_FAN))
	{
		if (pics & PIC_FAN)	LCD_Draw_Picture(pic3_Xmin, pic_Ymin, &fan_48x48[0]);
		else				LCD_ClearArea   (pic3_Xmin, pic_Ymin, pic3_Xmax, pic_Ymax, BackColor);
	}
#else
	if ((cour_pics & PIC_HOT1) != (pics & PIC_HOT1))
	{
		if(pics & PIC_HOT1)	LCD_Draw_Picture(pic1_Xmin, pic_Ymin, &extrude1_48x48[0]);
		else				LCD_Clear_Picture(pic1_Xmin, pic_Ymin);
	}
	if ((cour_pics & PIC_HOT2) != (pics & PIC_HOT2))
	{
		if(pics & PIC_HOT2)	LCD_Draw_Picture(pic2_Xmin, pic_Ymin, &extrude2_48x48[0]);
		else				LCD_Clear_Picture(pic2_Xmin, pic_Ymin);
	}
	if ((cour_pics & PIC_HOT3) != (pics & PIC_HOT3))
	{
		if(pics & PIC_HOT3)	LCD_Draw_Picture(pic3_Xmin, pic_Ymin, &extrude3_48x48[0]);
		else				LCD_Clear_Picture(pic3_Xmin, pic_Ymin);
	}
	if ((cour_pics & PIC_BED) != (pics & PIC_BED))
	{
	if (pics & PIC_BED)	LCD_Draw_Picture(pic4_Xmin, pic_Ymin, &bed_48x48[0]);
		else				LCD_Clear_Picture(pic4_Xmin, pic_Ymin);
	}
	if ((cour_pics & PIC_FAN) != (pics & PIC_FAN))
	{
		if (pics & PIC_FAN)	LCD_Draw_Picture(pic5_Xmin, pic_Ymin, &fan_48x48[0]);
		else				LCD_Clear_Picture(pic5_Xmin, pic_Ymin);
	}
#endif
	cour_pics = pics;
}
/***********************************************************
 Parse input commands
***********************************************************/
void handle_command()
{
	uint16_t x, y, i;

	new_command = 0;

	// screen update time is about 33...41ms
	switch(cmd)
	{
		case INIT:
			LCD_FillScreen(BackColor);
			for (i = 0; i < 48; i++)	{datat[i] = ' ';}
			for (i = 0; i < (FB_SIZE - 2); i++)	{data[i] = ' ';}
			data[FB_SIZE - 2] = cour_pics = 0;
			data[FB_SIZE - 1] = cour_leds = 0;
			break;

		case LCD_WRITE:
			i = 0;
			if (data[20] == 'X')	Get_Temps();	//main screen
			for (y = 0; y < TEXT_LINES; y++)
			{
				if ((y == 4) && ((data[20] != ' ') && (data[20] != '>')))
				{//main  or start screen
					DrawIcons();
					SetLeds();
					Print_Temps();
					y = TEXT_LINES;
				}
				else
				{
					LCD_SetCursor(0, y);
					for (x = 0; x < CHARS_PER_LINE; x++)
					{
						if (x == 0)
						{
							if (data[i] == '>')
							{//change menu cursor
								LCD_Set_TextColor(Black, White);
								data[i] = ' ';
							}
							else
								LCD_Set_TextColor(White, Black);
						}
						LCD_DrawChar(data[i++]);
					}
					CS_LCD_clr;
				}
			}
			if (buzzer_on)
			{
				TIM3->BUZZER_CCR = 127;
				buzzer_on = 0;
			}
			else
				TIM3->BUZZER_CCR = 0;
			break;

		case CONTRAST:
			TIM3->CONTRAST_CCR = data[0];
			break;
	}
}
//#################################################################################
void SPI2_IRQHandler(void)
{
	uint8_t b = SPI->DR;  // grab byte from SPI Data Register, reset Interrupt flag
	int16_t c;

	if (toread == 0)
	{// command
		switch(b)
		{
			case GET_SPI_DATA:	//for reading data
				return;
			case READ_BUTTONS:
				SPI->DR = Get_Buttons();
				return;
			case READ_ENCODER:
				c = TIM1->CNT;
				TIM1->CNT = 0;
				SPI->DR = (int8_t)c;
				return;
			case INIT:
				cmd = b;
				new_command = 1;
				return;
			case LCD_WRITE:
				cmd = b;
				toread = FB_SIZE;
				pos = 0;
				return;
			case BUZZER:
				buzzer_on = 1;
				return;
			case CONTRAST:
				cmd = b;
				toread = 1;
				pos = 0;
				return;
		}
	}
	else
	{
		data[pos++] = b;
		toread--;
		if (toread == 0)	new_command = 1;
	}
}
