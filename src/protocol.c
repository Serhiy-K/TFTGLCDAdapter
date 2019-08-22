#include "stm32f10x_i2c.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "LCD.h"
#include "Pictures.h"
#include "protocol.h"
#include "systick.h"

#define pic_Ymin	LCDYMAX - 48
//for one extruder config
#define	pic1_Xmin1	16
#define	pic2_Xmin1	96
#define	pic3_Xmin1	176
#define pic4_Xmin1	256
//for multy-extruders config
#define	pic1_Xmin	0
#define	pic2_Xmin	64
#define	pic3_Xmin	128
#define	pic4_Xmin	208
#define	pic5_Xmin	272
#ifdef ILI9325
#define pic6_Xmin	pic5_Xmin
#define pic6_Ymin	pic_Ymin - 60
#endif
#ifdef ILI9327
#define pic6_Xmin	336
#define pic6_Ymin	pic_Ymin
#endif

#define PROGRESS_COLOR	Yellow

#define FB_SIZE	(CHARS_PER_LINE * TEXT_LINES + 2)	//text area + leds + pics

enum Commands {
	GET_SPI_DATA = 0,	// is a read results of previous command
	READ_BUTTONS,
	READ_ENCODER,
	LCD_WRITE,
	BUZZER,
	CONTRAST,
	// Other commands... 0xE0 thru 0xFF
	GET_LCD_ROW = 0xE0,
	GET_LCD_COL,
	CLEAR_BUFFER,
	REDRAW,
	INIT = 0xFE,  // Initialize
};

// data[FB_SIZE - 1] - leds
// data[FB_SIZE - 2] - pics
uint8_t	data[FB_SIZE];
uint8_t	datat[48] = {' '};

uint8_t	cmd = 0;
int16_t pos = -1;
uint8_t toread = 0;
uint8_t	new_command = 0;
uint8_t	cour_pics = 0;
uint8_t	cour_leds = 0;
uint8_t	temps = 1;
uint8_t	cour_row = 0;
uint16_t row_offset = 0;
uint8_t progress_cleared = 0;
uint8_t	protocol = Smoothie;
uint16_t freq = 0, duration = 0;
uint16_t dot_pos_x = CHAR_WIDTH;
uint16_t dot_pos_y = 7 * CHAR_HEIGTH;
int8_t encdiff;
uint8_t c_p = 0;

//----------------------------------------------------------------------------
uint8_t New_cmd()
{
	return new_command;
}
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void Get_Temps()
{
	for (uint8_t i = 0; i < 20; i++)
	{
		//scan first line for temperatures
		if (data[i] == ':')
		{
			switch (data[i-1])
			{
			case 'T':	//by default
			case 'E':	//extruder
			case 'H':	//hotend
				if (temps == 1)
				{	//when first step and hotend present
					datat[2] = 'H';	datat[3] = 'E';
					Rem_Lead_Zero(i);
					datat[17] = data[i+1];	datat[18] = data[i+2];	datat[19] = data[i+3];
					datat[33] = data[i+5];	datat[34] = data[i+6];	datat[35] = data[i+7];
				}
				else
				{
					datat[0] = 'H';	datat[1] = 'E';	datat[2] = '1';
					Rem_Lead_Zero(i);
					datat[16] = data[i+1];	datat[17] = data[i+2];	datat[18] = data[i+3];
					datat[32] = data[i+5];	datat[33] = data[i+6];	datat[34] = data[i+7];
				}
				break;
			case 'B':
				if (temps == 1)
				{	//when first step and one hotend
					datat[6] = 'B';	datat[7] = 'E';	datat[8] = 'D';
					Rem_Lead_Zero(i);
					datat[22] = data[i+1];	datat[23] = data[i+2];	datat[24] = data[i+3];
					datat[38] = data[i+5];	datat[39] = data[i+6];	datat[40] = data[i+7];
				}
				else
				{
					datat[13] = 'B';	datat[14] = 'E';	datat[15] = 'D';
					Rem_Lead_Zero(i);
					datat[29] = data[i+1];	datat[30] = data[i+2];	datat[31] = data[i+3];
					datat[45] = data[i+5];	datat[46] = data[i+6];	datat[47] = data[i+7];
				}
				break;
			case '2':
				datat[4] = 'H';	datat[5] = 'E';	datat[6] = '2';
				if (temps == 1)
				{ // when first step
					temps = 3;
					uint8_t j;
					//clean previous data
					for (j = 0; j < 9; j++ ) datat[j] = ' ';
					for (j = 16; j < 25; j++ ) datat[j] = ' ';
					for (j = 32; j < 41; j++ ) datat[j] = ' ';
				}
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
			}
		}
	}
	for (uint8_t i = 0; i < 20; i++)	{data[i] = ' ';}
}
//----------------------------------------------------------------------------
void Print_Temps()
{
	uint8_t i, x, y;
	i = 0;
	//main screen
	if (protocol == Smoothie)
	{
		for (y = 5; y < 8; y++)
		{
			LCD_SetCursor(0, y);
			for (x = 0; x < 16; x++)
			{
				LCD_DrawChar(datat[i++]);
			}
		}
	}
	else if (protocol == Marlin1)
	{
		LCD_SetCursor(0, 5);	for (x = 0; x < 16; x++)	LCD_DrawChar(data[100 + x]);
		LCD_SetCursor(0, 6);	for (x = 0; x < 16; x++)	LCD_DrawChar(data[120 + x]);
		LCD_SetCursor(0, 7);	for (x = 0; x < 16; x++)	LCD_DrawChar(data[140 + x]);
		if (data[102] == '1') temps = 3;
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
	else if (protocol == Marlin1)
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
		TIM3->BUZZER_CCR = 127;
		TIM2->CR1 |= TIM_CR1_CEN;
	}
}
//----------------------------------------------------------------------------
void SetLeds()
{
	uint8_t leds;

	leds = data[FB_SIZE - 1];
	leds &= LED_MASK;

	if (((cour_leds & LED_HOT) != (leds & LED_HOT)) && !(cour_pics & PIC_LOGO))
	{
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
	}
	cour_leds = leds;
}
//----------------------------------------------------------------------------
void DrawIcons()
{
	const char border[] = {196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 0};

	uint8_t pics;

	pics = data[FB_SIZE - 2];
	pics &= PIC_MASK;

	if (pics & PIC_LOGO)
	{	//Text Logo
		LCD_Set_TextColor(Yellow, Blue);
		if (protocol == Smoothie)
		{	//only on SPI bus
		    NVIC_DisableIRQ(I2C_IRQ);
		    NVIC_DisableIRQ(I2C_ERR_IRQ);
		    I2C_Cmd(I2C, DISABLE);

			LCD_DrawChar_XY(4, 4, 218);	LCD_PutStrig(&border[0]);	LCD_DrawChar(191);	CS_LCD_set;
			LCD_DrawChar_XY(4, 5, 179);	LCD_PutStrig("   Open   ");	LCD_DrawChar(179);	CS_LCD_set;
			LCD_DrawChar_XY(4, 6, 179);	LCD_PutStrig(" Hardware ");	LCD_DrawChar(179);	CS_LCD_set;
			LCD_DrawChar_XY(4, 7, 192);	LCD_PutStrig(&border[0]);	LCD_DrawChar(217);	CS_LCD_set;
		}
		else if (protocol == Marlin1)
		{	//Text Logo from Marlin
			NVIC_DisableIRQ(SPI_IRQ);
			SPI_Cmd(SPI, DISABLE);

			uint8_t i = 0;
			for (uint8_t y = 0; y < 4; y++)
			{
				LCD_SetCursor(0, y);
				for (uint8_t x = 0; x < CHARS_PER_LINE; x++)
				{
					if (data[i] == '-')	{
						LCD_DrawChar(11); i++;
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
	}

	if (temps == 1)
	{
		if ((cour_pics & PIC_HE1) != (pics & PIC_HE1))
		{
			if (pics & PIC_HE1) LCD_Draw_Picture (pic1_Xmin1, pic_Ymin, &extrude_48x48[0]);
			else				LCD_Clear_Picture(pic1_Xmin1, pic_Ymin);
		}
		if ((cour_pics & PIC_BED) != (pics & PIC_BED))
		{
			if (pics & PIC_BED)	LCD_Draw_Picture (pic2_Xmin1, pic_Ymin, &bed_48x48[0]);
			else				LCD_Clear_Picture(pic2_Xmin1, pic_Ymin);
		}
		if ((cour_pics & PIC_FAN) != (pics & PIC_FAN))
		{
			if (pics & PIC_FAN)	LCD_Draw_Picture (pic3_Xmin1, pic_Ymin, &fan_48x48[0]);
			else				LCD_Clear_Picture(pic3_Xmin1, pic_Ymin);
		}
		if ((cour_pics & PIC_HOT) != (pics & PIC_HOT))
		{
			if (pics & PIC_HOT)	LCD_Draw_Picture (pic4_Xmin1, pic_Ymin, &heat_48x48[0]);
			else				LCD_Clear_Picture(pic4_Xmin1, pic_Ymin);
		}
	}
	else
	{
		if ((cour_pics & PIC_HE1) != (pics & PIC_HE1))
		{
			if (pics & PIC_HE1)	LCD_Draw_Picture (pic1_Xmin, pic_Ymin, &extrude1_48x48[0]);
			else				LCD_Clear_Picture(pic1_Xmin, pic_Ymin);
		}
		if ((cour_pics & PIC_HE2) != (pics & PIC_HE2))
		{
			if (pics & PIC_HE2)	LCD_Draw_Picture (pic2_Xmin, pic_Ymin, &extrude2_48x48[0]);
			else				LCD_Clear_Picture(pic2_Xmin, pic_Ymin);
		}
		if ((cour_pics & PIC_HE3) != (pics & PIC_HE3))
		{
			if (pics & PIC_HE3)	LCD_Draw_Picture (pic3_Xmin, pic_Ymin, &extrude3_48x48[0]);
			else				LCD_Clear_Picture(pic3_Xmin, pic_Ymin);
		}
		if ((cour_pics & PIC_BED) != (pics & PIC_BED))
		{
		if (pics & PIC_BED)		LCD_Draw_Picture (pic4_Xmin, pic_Ymin, &bed_48x48[0]);
			else				LCD_Clear_Picture(pic4_Xmin, pic_Ymin);
		}
		if ((cour_pics & PIC_FAN) != (pics & PIC_FAN))
		{
			if (pics & PIC_FAN)	LCD_Draw_Picture (pic5_Xmin, pic_Ymin, &fan_48x48[0]);
			else				LCD_Clear_Picture(pic5_Xmin, pic_Ymin);
		}
		if ((cour_pics & PIC_HOT) != (pics & PIC_HOT))
		{
			if (pics & PIC_HOT)	LCD_Draw_Picture (pic6_Xmin, pic6_Ymin, &heat_48x48[0]);
			else				LCD_Clear_Picture(pic6_Xmin, pic6_Ymin);
		}
	}
	cour_pics = pics;
}
//----------------------------------------------------------------------------
void UBL_Draw_Dot()
{
	uint8_t i, grid_points_x, grid_points_y;
	uint8_t point_x, point_y;	//point_y - inverted
	uint8_t step_x, step_y;

	grid_points_x = data[FB_SIZE - 1] >> 4;
	grid_points_y = data[FB_SIZE - 1] & 0x0f;

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
uint8_t  Get_Progress()
{
	uint8_t percent;
	percent = data[58] - '0';	//fixed position
	if (data[57] != ' ')	percent += (data[57] - '0') * 10;
	if (data[56] == '1')	percent = 100;
	return percent;
}
//----------------------------------------------------------------------------
void Draw_Progress_Bar(uint8_t y, uint8_t percent)
{
	uint8_t i, ymin, ymax;

	const uint16_t pb_colors[] = {0xfc00, 0xfcc0, 0xfd80, 0xfe40, 0xff20, 0xffe0, 0xcfe0, 0x97e0, 0x67e0, 0x37e0, 0x07e0};

#define XMIN	(LCDXMAX - 302) / 2	//center progress bar to text line
#define XMAX	XMIN + 302			//100% * 3 + 2 for frame

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

	i = percent / 10;
	//draw progress bar frame
	LCD_ClearArea(XMIN, ymin, XMIN, ymax, PROGRESS_COLOR);			// left |
	LCD_ClearArea(XMIN + 1, ymin, XMAX, ymin, PROGRESS_COLOR);		// top -
	LCD_ClearArea(XMIN + 1, ymax, XMAX, ymax, PROGRESS_COLOR);		// bot -
	LCD_ClearArea(XMAX, ymin + 1, XMAX, ymax - 1, PROGRESS_COLOR);	// right |

	if (c_p != percent)
	{//change progressbar
		if (percent > c_p) //draw progress bar
			LCD_ClearArea(XMIN + 1, ymin + 1, XMIN + 1 + percent * 3, ymax - 1, pb_colors[i]);
		else
			LCD_ClearArea(XMIN + 1, ymin + 1, XMAX - 1, ymax - 1, Black);
		c_p = percent;
	}
}
//----------------------------------------------------------------------------
uint16_t Print_Line(uint16_t i, uint8_t row)
{
	if ((data[i] == '>') || (data[i] == 0x03))
	{//change menu cursor
		progress_cleared = 0;
		LCD_Set_TextColor(Black, White);
	}
	else
		LCD_Set_TextColor(White, Black);

	LCD_SetCursor(0, row);
	for (uint8_t x = 0; x < CHARS_PER_LINE; x++)
	{
		if ((x == 0) && (data[i] == '>'))
			LCD_DrawChar(' '), i++, x++;
		LCD_DrawChar(data[i++]);
	}
	CS_LCD_set;
	return i;
}
//----------------------------------------------------------------------------
void handle_command()
{
	uint16_t x, y, i;

	//update time for screen is about 33...41ms
	switch(cmd)
	{
		case INIT:
			protocol = data[0];
			progress_cleared = 0;
			LCD_FillScreen(BackColor);

		case CLEAR_BUFFER:
			for (i = 0; i < 48; i++)	datat[i] = ' ';
			for (i = 0; i < (FB_SIZE - 2); i++)	data[i] = ' ';
			data[FB_SIZE - 2] = cour_pics = 0;
			data[FB_SIZE - 1] = cour_leds = 0;
			break;

		case LCD_WRITE:
			i = 0;
			if (protocol == Smoothie)
			{
				if (data[20] == 'X')	Get_Temps();	//main screen

				for (y = 0; y < TEXT_LINES; y++)
				{
					if ((y == 4) && ((data[20] != ' ') && (data[20] != '>')))
					{//main or start screen
						if ((data[FB_SIZE - 2] && PIC_LOGO) == 0)
							Draw_Progress_Bar(y, Get_Progress());
						Print_Temps();
						DrawIcons();
						SetLeds();
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
									progress_cleared = 0;
									LCD_Set_TextColor(Black, White);
									data[i] = ' ';
								}
								else
									LCD_Set_TextColor(White, Black);
							}
							LCD_DrawChar(data[i++]);
						}
						CS_LCD_set;
					}
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
						for (y = 0; y < TEXT_LINES; y++)
						{
							if (data[i] == '%')
							{
								Draw_Progress_Bar(y, data[i + 1]);
								i += CHARS_PER_LINE;
							}
							else if ((y == 5) && (data[0] == 'X'))
							{//main screen
								Print_Temps();
								DrawIcons();
								y = TEXT_LINES;
							}
							else
								i = Print_Line(i, y);
						}
						if (data[0] == 218)	UBL_Draw_Dot();
					}
					else
					{//print one line
						i = row_offset;
						if (data[i] == '%')	Draw_Progress_Bar(cour_row, data[i + 1]);
						else				Print_Line(i, cour_row);
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
	    	if (cmd == REDRAW)
	    	{
	    		cmd = LCD_WRITE;
	    		pos = FB_SIZE;
	    	}
	    	while ((I2C->SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR) { I2C->SR1; I2C->SR2; }	// ADDR-Flag clear
	    	while ((I2C->SR1 & I2C_SR1_STOPF) == I2C_SR1_STOPF) { I2C->SR1; I2C->CR1 |= 0x1; }	// STOPF Flag clear
	    	break;
	    // Slave TRANSMITTER mode
	    case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:	//EV1
	    	switch (cmd)
	    	{
	    		case READ_BUTTONS:	I2C->DR = Get_Buttons();	break;
	    		case READ_ENCODER:	encdiff = (int8_t)TIM1->CNT;	TIM1->CNT = 0;	I2C->DR = encdiff;	break;
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
			case READ_ENCODER:	c = TIM1->CNT;	TIM1->CNT = 0;	SPI->DR = (int8_t)c; return;
			case LCD_WRITE:		cmd = b;	toread = FB_SIZE;	pos = 0;	return;
			case BUZZER:		cmd = b;	toread = 4;	pos = 0;	return;
			case CONTRAST:		cmd = b;	toread = 1;	pos = 0;	return;
			case GET_LCD_ROW:	SPI->DR = TEXT_LINES;	return;
			case GET_LCD_COL:	SPI->DR = CHARS_PER_LINE;	return;
			case CLEAR_BUFFER:	cmd = b;	new_command = 1;	return;
			case REDRAW: 		cmd = LCD_WRITE;	toread = 0; new_command = 1;	return;
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
