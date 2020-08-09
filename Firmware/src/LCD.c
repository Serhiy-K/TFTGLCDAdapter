/******************************************************************************
 Graphics Library, 8-bit parallel data bus
 Support for ILI9325, ILI9327 and ILI9341
******************************************************************************/
#include "LCD.h"
#include "Pictures.h"
#include "systick.h"

#ifndef HW_VER_2
volatile uint16_t TCH, TCL;
volatile uint16_t BCH, BCL;
uint16_t BH = (BackColor >> 8);
uint16_t BL = (BackColor & 0x00ff);
#endif
uint8_t orientation = 0;	//0 - LCD chip right, 1 - LCD chip left
volatile uint16_t TBack, TColor;
uint16_t Back = BackColor;

uint16_t Xcour;
//extern uint8_t	protocol;

/*
// For read LCD ID

uint16_t LCD_reg_data[8] = {0};
char LCD_REG_HEX[8][4];
char LCD_REG[2];
char HEX[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
uint8_t nz;
void LCD_Read_Reg(uint8_t RegisterIndex)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	uint8_t i, t;
	uint16_t j;

	nz = 0;

	RS_LCD_clr;
#ifndef HW_VER_2
	LCD_DATA_PORT->BRR = LCD_DATA_MASK;
	WR_Puls;
	LCD_DATA_PORT->BSRR = RegisterIndex;
	WR_Puls;
#else
	LCD_DATA(RegisterIndex);
#endif
	RS_LCD_set;

	LCD_REG[1] = HEX[RegisterIndex & 0x0f];
	LCD_REG[0] = HEX[RegisterIndex >> 4];

	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin	 = LCD_DATA_MASK;
	GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);

	for (i = 0; i < 8; i++)
	{
		RD_LCD_clr;
#ifndef HW_VER_2
		LCD_reg_data[i] = LCD_DATA_PORT->IDR & 0xff;
		LCD_reg_data[i] <<= 8;
		RD_LCD_set;
		RD_LCD_clr;
		LCD_reg_data[i] |= LCD_DATA_PORT->IDR & 0xff;

		if (LCD_reg_data[i])
			nz = 1;
#else
		LCD_reg_data[i] = LCD_DATA_PORT->IDR;
#endif
		RD_LCD_set;
		j = LCD_reg_data[i];
		t = j;	t &= 0x0f;	LCD_REG_HEX[i][3] = HEX[t];	j >>= 4;
		t = j;	t &= 0x0f;	LCD_REG_HEX[i][2] = HEX[t];	j >>= 4;
		t = j;	t &= 0x0f;	LCD_REG_HEX[i][1] = HEX[t];	j >>= 4;
		t = j;	t &= 0x0f;	LCD_REG_HEX[i][0] = HEX[t];
	}

	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);
}
void LCD_Read_Regs()
{
	uint8_t i;
	for (i = 0; i < 255; i++)
	{
		LCD_Read_Reg(i);
		if (nz)
		{
				LCD_reg_data[8] = 0;
		}
	}
}
*/
#if defined(ILI9325)
/******************************************************************************
 Description    : Write data to LCD reg
 Input          : RegisterIndex, data
******************************************************************************/
static void Set_LCD_REG(uint8_t RegisterIndex, uint16_t Data)
{
	RS_LCD_clr;
#ifndef HW_VER_2
	LCD_DATA_PORT->BRR = LCD_DATA_MASK;	 WR_Puls;
	LCD_DATA_PORT->BSRR = RegisterIndex; WR_Puls;
#else
	LCD_DATA(RegisterIndex);
#endif
	RS_LCD_set;

#ifndef HW_VER_2
	LCD_DATA(Data >> 8);
	LCD_DATA(Data & 0x00ff);
#else
	LCD_DATA(Data);
#endif
}
#endif

#if defined(ILI9327) || defined(ILI9341) || defined(ST7789)
/******************************************************************************
 Description    : Write data to LCD reg
 Input          : RegisterIndex
******************************************************************************/
static void LCD_Write_Com(uint8_t RegisterIndex)
{
	RS_LCD_clr;
	LCD_DATA(RegisterIndex);
	RS_LCD_set;
}
#endif
/******************************************************************************
 Description    : Set output area
 Input          : X0pos, Y0pos, X1pos, Y1pos
******************************************************************************/
void LCD_SetArea(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1)
{
	CS_LCD_clr;

#ifdef ILI9325
	Set_LCD_REG(0x52, X0);
	Set_LCD_REG(0x53, X1);
	Set_LCD_REG(0x50, Y0);
	Set_LCD_REG(0x51, Y1);

	Set_LCD_REG(0x21, X0);
	Set_LCD_REG(0x20, Y1);
#endif	//ILI9325
#ifdef ILI9327
	LCD_Write_Com(0x2A);
	LCD_DATA(((LCDYMAX - 1) - Y1) >> 8);
	LCD_DATA((LCDYMAX - 1) - Y1);
	LCD_DATA(((LCDYMAX - 1) - Y0) >> 8);
	LCD_DATA((LCDYMAX - 1) - Y0);
	LCD_Write_Com(0x2B);
	if (orientation)
	{
		LCD_DATA((X0 + 32) >> 8);
		LCD_DATA(X0 + 32);
		LCD_DATA((X1 + 32) >> 8);
		LCD_DATA(X1 + 32);
	}
	else
	{
		LCD_DATA((X0) >> 8);
		LCD_DATA(X0);
		LCD_DATA((X1) >> 8);
		LCD_DATA(X1);
	}
#endif	//ILI9327
#if defined(ILI9341) || defined(ST7789)
	LCD_Write_Com(0x2A);
	LCD_DATA(((LCDYMAX - 1) - Y1) >> 8);
	LCD_DATA((LCDYMAX - 1) - Y1);
	LCD_DATA(((LCDYMAX - 1) - Y0) >> 8);
	LCD_DATA((LCDYMAX - 1) - Y0);
	LCD_Write_Com(0x2B);
	LCD_DATA(X0 >> 8);
	LCD_DATA(X0);
	LCD_DATA(X1 >> 8);
	LCD_DATA(X1);
#endif

	RS_LCD_clr;
	//lcd_Draw_Start
#if defined(ILI9325)
#ifndef HW_VER_2
	LCD_DATA_PORT->BRR = LCD_DATA_MASK;	WR_Puls;
	LCD_DATA_PORT->BSRR = 0x22; WR_Puls;
#else
	LCD_DATA(0x22);
#endif
#endif	//ILI9325

#if defined(ILI9327) || defined(ILI9341) || defined(ST7789)
	LCD_DATA(0x2C);
#endif
	RS_LCD_set;
}
/******************************************************************************
 Description    : Fill screen with back color
******************************************************************************/
void LCD_ClearScreen()
{
    uint32_t cntT = 0;

    LCD_SetArea(0, 0, LCDXMAX - 1, LCDYMAX - 1);
#ifndef HW_VER_2
	if (Back)
	{
		do	{LCD_DATA(BH);	LCD_DATA(BL);} while (++cntT < (LCDXMAX * LCDYMAX));
	}
	else
	{
		LCD_DATA_PORT->BRR = LCD_DATA_MASK; 
		do {WR_Puls;	WR_Puls;} while (++cntT < (LCDXMAX * LCDYMAX));
	}
#else
	LCD_DATA_PORT->ODR = Back; 
	do {WR_Puls;} while (++cntT < (LCDXMAX * LCDYMAX));
#endif
    CS_LCD_set;
}
/******************************************************************************
 Description    : Fill screen area with one color
 Input          : X0pos, Y0pos, X1pos, Y1pos, Color
******************************************************************************/
void LCD_FillRect(uint16_t X0pos, uint16_t Y0pos, uint16_t X1pos, uint16_t Y1pos, uint16_t Color)
{
	uint32_t pntNum;

	pntNum = ((X1pos - X0pos) + 1) * ((Y1pos - Y0pos) + 1);	// calculate dots number
	LCD_SetArea(X0pos, Y0pos, X1pos, Y1pos);
#ifndef HW_VER_2
	uint16_t CH = Color >> 8;
	uint16_t CL = Color & 0x00ff;

	if (Color == 0xffff)
	{
		LCD_DATA_PORT->BSRR = LCD_DATA_MASK;
		do {WR_Puls; WR_Puls;} while (pntNum--);
	}
	else if (Color == 0)
	{
		LCD_DATA_PORT->BRR = LCD_DATA_MASK; 
		do {WR_Puls; WR_Puls;} while (pntNum--);
	}
	else
	{
		do {LCD_DATA(CH);	LCD_DATA(CL);}	while (pntNum--);
	}
#else
	LCD_DATA_PORT->ODR = Color; 
	do {WR_Puls;} while (pntNum--);
#endif
	CS_LCD_set;
}
/******************************************************************************
 Description    : Draw picture 48x48
 Input          : X0pos, Y0pos, *pic
******************************************************************************/
void LCD_Draw_Picture(uint16_t X0pos, uint16_t Y0pos, const char *pic)
{
	uint16_t i;
	uint8_t c;

	LCD_SetArea(X0pos, Y0pos, X0pos + 47, Y0pos + 47 );
	for (i = 0; i < 2304; i++)
	{
		c = *pic - 'A';
#ifndef HW_VER_2
		if		(colors[c] == 0xffff)	{LCD_DATA_FF;}
		else if	(colors[c] == 0)		{LCD_DATA_00;}
		else							{LCD_DATA(colors[c] >> 8);	LCD_DATA(colors[c] & 0x00ff);}
#else
		LCD_DATA(colors[c]);
#endif
		pic++;
	}
	CS_LCD_set;
}
/******************************************************************************
 Description    : Erase picture 48x48
 Input          : X0pos, Y0pos
******************************************************************************/
void LCD_Clear_Picture(uint16_t X0pos, uint16_t Y0pos)
{
	uint16_t i;

	LCD_SetArea(X0pos, Y0pos, X0pos + 47, Y0pos + 47 );

#ifndef HW_VER_2
	if (Back)
	{
		for (i = 0; i < 2304; i++)	{LCD_DATA(BH);	LCD_DATA(BL);}
	}
	else
	{
		LCD_DATA_PORT->BRR = LCD_DATA_MASK; 
		for (i = 0; i < 2304; i++) {WR_Puls;	WR_Puls;}
	}
#else
	LCD_DATA_PORT->ODR = Back; 
	for (i = 0; i < 2304; i++)	{WR_Puls;}
#endif
	CS_LCD_set;
}
/******************************************************************************
 Set color for text and back
******************************************************************************/
void LCD_Set_TextColor(uint16_t front, uint16_t back)
{
	TColor = front;
	TBack = back;
#ifndef HW_VER_2
	TCH = front >> 8;
	TCL = front & 0x00ff;
	BCH = back >> 8;
	BCL = back & 0x00ff;
#endif
}
/******************************************************************************
 Description    : Set cursor for text
 Input          : Xpos, Ypos
******************************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	//count from top left corner
	uint16_t Xmin = Xpos * CHAR_WIDTH;
	uint16_t Ymin = Ypos * CHAR_HEIGTH;
	uint16_t Ymax = Ymin + (CHAR_HEIGTH - 1);
	//area from start symbol to end of line
	LCD_SetArea(Xmin, Ymin, LCDXMAX - 1, Ymax );
	Xcour = Xpos;
}
/******************************************************************************
 Description    : Convert Russian char from HD44780 encoding to ASCII
 Input          : HD44780 char c
 Output         : ASCII char c
******************************************************************************/
/*
uint8_t HD44780_to_ASCII(char c)
{
	const uint8_t transcode[] =
	{	//symbols for cyrillic
		129, 131, 240, 134, 135, 136, 137, 139, 143, 147, 148, 151, 152, 154, 155, 157,
		158, 159, 161, 162, 163, 241, 166, 167, 168, 169, 170, 171, 172, 173, 175, 226,
		231, 232, 234, 235, 236, 237, 238, 239,
		132, 150, 153, 164, 228, 230, 233
	};
	if ((c >= 0xa0) && (c <= 0xc7))
		return transcode[c - 0xa0];
	else if ((c >= 0xe0) && (c <= 0xe6))
		return transcode[c - 0xb8];
	else
		return c;
}
*/
/******************************************************************************
 Description    : Draw char to current position
 Input          : char c
******************************************************************************/
void LCD_DrawChar(char c)
{
	uint8_t i, j;
	const char *ptr; // pointer to char data
	uint8_t mask;

	//	if ((protocol == MarlinI2C) || (protocol == MarlinSPI))
	//		c = HD44780_to_ASCII(c);
	ptr = &FONT[(uint8_t)c][0];
	for (i = 0; i < CHAR_BYTES; i++)
	{
		mask = 0b10000000;
#ifndef HW_VER_2
		if ((TColor == 0xffff) && (TBack == 0))	//standart text
		{
			for (j = 0; j < 8; j++)
			{
				if (*ptr & mask)	{LCD_DATA_FF;}
				else				{LCD_DATA_00;}
				mask = mask >> 1;
			}
		}
		else if ((TColor == 0) && (TBack == 0xffff))	//standart text (cursor)
		{
			for (j = 0; j < 8; j++)
			{
				if (*ptr & mask)	{LCD_DATA_00;}
				else				{LCD_DATA_FF;}
				mask = mask >> 1;
			}
		}
		else
		{
			for (j = 0; j < 8; j++)
			{
				if (*ptr & mask)	{LCD_DATA(TCH);	LCD_DATA(TCL);}
				else				{LCD_DATA(BCH);	LCD_DATA(BCL);}
				mask = mask >> 1;
			}
		}
#else
		for (j = 0; j < 8; j++)
		{
			if (*ptr & mask)	{LCD_DATA(TColor);}
			else				{LCD_DATA(TBack);}
			mask = mask >> 1;
		}
#endif
		ptr++;
	}
	Xcour++;	//increment cursor position
}
/******************************************************************************
 Description    : Draw char to specified position
 Input          : uint16_t XPos, uint16_t YPos, const char c
******************************************************************************/
void LCD_DrawChar_XY(uint16_t XPos, uint16_t YPos, char c)
{
	if (XPos >= CHARS_PER_LINE) return;
	if (YPos >= TEXT_LINES) return;

	LCD_SetCursor(XPos, YPos);
	LCD_DrawChar(c);
}
/******************************************************************************
 Description    : Draw text string
 Input          : Pointer to string *str
******************************************************************************/
void LCD_PutStrig(char *str)
{
	while (*str != 0)
	{
		if (Xcour >= CHARS_PER_LINE) break;
		LCD_DrawChar(*str);
		str++;		// next symbol
	}
}
/******************************************************************************
 Description    : Draw text string to specified position
 Input          : XPos, YPos, *str
******************************************************************************/
void LCD_PutStrig_XY(uint16_t XPos, uint16_t YPos, char *str)
{
	if (XPos >= CHARS_PER_LINE) return;
	if (YPos >= TEXT_LINES) return;

	LCD_SetCursor(XPos, YPos);
	LCD_PutStrig(str);
	CS_LCD_set;
}
/******************************************************************************
 Description    : Draw Start Screen
******************************************************************************/
void LCD_Draw_StartScreen()
{
#ifdef LCD320x240
	LCD_ClearScreen();
	LCD_Set_TextColor(Yellow, Blue);
	LCD_PutStrig_XY(0, 0, "    3-D Printer     ");
	LCD_PutStrig_XY(0, 1, " 320x240 TFT Panel  ");
#ifndef HW_VER_2
	LCD_PutStrig_XY(0, 2, "      Ver.1.1       ");
#else
	LCD_PutStrig_XY(0, 2, "      Ver.2.0       ");
#endif
	LCD_Set_TextColor(White, BackColor);
	LCD_PutStrig_XY(0, 4, "Waiting for printer ");
	LCD_PutStrig_XY(4, 5,     "connection..");
#else
	//clear 400x240 dots
	LCD_ClearArea(0, 0, 399, LCDYMAX - 1, BackColor);
	LCD_Set_TextColor(Yellow, Blue);
	LCD_PutStrig_XY(0, 0, "      3-D Printer       ");
	LCD_PutStrig_XY(0, 1, "   400x240 TFT Panel    ");
#ifndef HW_VER_2
	LCD_PutStrig_XY(0, 2, "        Ver.1.1         ");
#else
	LCD_PutStrig_XY(0, 2, "        Ver.2.0         ");
#endif
	LCD_Set_TextColor(White, BackColor);
	LCD_PutStrig_XY(0, 4, "   Waiting for printer  ");
	LCD_PutStrig_XY(6, 5,       "connection...");
#endif
}
/******************************************************************************
 Description    : Reset LCD and prepare for 8-bit interface
******************************************************************************/
void LCD_Reset()
{
	// Reset chip
   	RES_LCD_clr;
   	delay_ms(10);
   	RES_LCD_set;
   	delay_ms(100);	// Wait Stability
	CS_LCD_clr;

#ifndef HW_VER_2
   	//for 8-bit interface
   	RS_LCD_clr;
   	LCD_DATA_PORT->BRR = LCD_DATA_MASK;	WR_Puls;	WR_Puls;	WR_Puls;	WR_Puls;
   	RS_LCD_set
#endif
}

#ifdef ST7789
/******************************************************************************
* Function Name  : LCD_Init
* Description    : Init ST7789 compatible chip
******************************************************************************/
void LCD_Init(void)
{
	LCD_Reset();

	LCD_Write_Com(0x01);	// Software reset

	delay_ms(10);

	LCD_Write_Com(0xb2);	// Porch Control
	LCD_DATA(0x0c);	LCD_DATA(0x0c);	LCD_DATA(0x00);
	LCD_DATA(0x33);	LCD_DATA(0x33);
	LCD_Write_Com(0x3a);	LCD_DATA(0x55);	// Interface Pixel Format
	LCD_Write_Com(0xbb);	LCD_DATA(0x2A);	// VCOM Setting
	LCD_Write_Com(0xc3);	LCD_DATA(0x0a);	// VRH Setting
	LCD_Write_Com(0xc4);	LCD_DATA(0x20);	// VDV Setting
	LCD_Write_Com(0xc6);	LCD_DATA(0x0f);	// Frame Rate Control in Normal Mode
	LCD_Write_Com(0x0d);	// Power Control 1
	LCD_DATA(0xa4);	LCD_DATA(0xa2);
	LCD_Write_Com(0xd0);	// Power Control 2
	LCD_DATA(0xa4);	LCD_DATA(0xa2);
	LCD_Write_Com(0x53);	LCD_DATA(0x24); // CTRL Display
	LCD_Write_Com(0xb7);	LCD_DATA(0x35);	// Gate Control
	LCD_Write_Com(0xe0);	// Positive Voltage Gamma Control
	LCD_DATA(0xd0);	LCD_DATA(0x00);	LCD_DATA(0x02);
	LCD_DATA(0x07);	LCD_DATA(0x0a);	LCD_DATA(0x28);
	LCD_DATA(0x32);	LCD_DATA(0x44);	LCD_DATA(0x42);
	LCD_DATA(0x06);	LCD_DATA(0x0e);	LCD_DATA(0x12);
	LCD_DATA(0x14);	LCD_DATA(0x17);
	LCD_Write_Com(0xe1);	// Negative Voltage Gamma Control
	LCD_DATA(0xd0);	LCD_DATA(0x00);	LCD_DATA(0x02);
	LCD_DATA(0x07);	LCD_DATA(0x0a);	LCD_DATA(0x28);
	LCD_DATA(0x31);	LCD_DATA(0x54);	LCD_DATA(0x47);
	LCD_DATA(0x0e);	LCD_DATA(0x1c);	LCD_DATA(0x17);
	LCD_DATA(0x1b);	LCD_DATA(0x1e);
	LCD_Write_Com(0xe4);	// Gate Control
	LCD_DATA(0x27);	//320 lines
	LCD_DATA(0x00);	//from 0 line
	LCD_DATA(0x00);	//SM = 0, GS = 0
	LCD_Write_Com(0x36);	// Memory Data Access Control
	if (orientation)
		{LCD_DATA(0x88);}	// BGR
	else
		{LCD_DATA(0x48);}	// BGR
	LCD_Write_Com(0x21);	// Display Inversion On
	LCD_Write_Com(0x13);	// Normal Display Mode On
	LCD_Write_Com(0x11);	// Exit Sleep Mode

	delay_ms(120);

	LCD_Write_Com(0x29);	// Display ON

	LCD_Draw_StartScreen();
}
#endif
#ifdef ILI9325
/******************************************************************************
* Function Name  : LCD_Init
* Description    : Init ILI9325 chip
******************************************************************************/
void LCD_Init(void)
{
	LCD_Reset();

	Set_LCD_REG(0x02, 0x0200); // set 1 line inversion
	if (orientation)
	{
   		Set_LCD_REG(0x01, 0x0100); // set SS=1 and SM=0 bit
   		Set_LCD_REG(0x60, 0x2700); // set GS=0 bit
	}
	else
	{
   		Set_LCD_REG(0x01, 0x0000); // set SS=0 and SM=0 bit
   		Set_LCD_REG(0x60, 0xA700); // set GS bit
	}
   	//xx0 BGR 0000 ORG 0 ID1 ID0 AM 000
   	Set_LCD_REG(0x03, 0x1020); // set BGR=1, GRAM write direction ID=2.
   	Set_LCD_REG(0x04, 0x0000); // Resize register
   	Set_LCD_REG(0x08, 0x0207); // set the back porch and front porch
   	Set_LCD_REG(0x09, 0x0000); // set non-display area refresh cycle ISC[3:0]
   	Set_LCD_REG(0x0A, 0x0000); // FMARK function
   	Set_LCD_REG(0x0C, 0x0000); // 16 bit RGB interface
   	Set_LCD_REG(0x0D, 0x0000); // Frame marker Position
   	Set_LCD_REG(0x0F, 0x0000); // RGB interface polarity

   	// Power On sequence
   	Set_LCD_REG(0x10, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
   	Set_LCD_REG(0x11, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
   	Set_LCD_REG(0x12, 0x0000); // VREG1OUT voltage
   	Set_LCD_REG(0x13, 0x0000); // VDV[4:0] for VCOM amplitude
   	Set_LCD_REG(0x07, 0x0001);
   	delay_ms(200); // Dis-charge capacitor power voltage
   	Set_LCD_REG(0x10, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
   	Set_LCD_REG(0x11, 0x0227); // Set DC1[2:0], DC0[2:0], VC[2:0]
   	delay_ms(50); // Delay 50ms
   	Set_LCD_REG(0x12, 0x000D); // 0012
   	delay_ms(50); // Delay 50ms
   	Set_LCD_REG(0x13, 0x1200); // VDV[4:0] for VCOM amplitude
   	Set_LCD_REG(0x29, 0x000A); // 04  VCM[5:0] for VCOMH
   	Set_LCD_REG(0x2B, 0x000D); // Set Frame Rate
   	delay_ms(50); // Delay 50ms
   	Set_LCD_REG(0x20, 0x0000); // GRAM horizontal Address
   	Set_LCD_REG(0x21, 0x0000); // GRAM Vertical Address
   	// ----------- Adjust the Gamma Curve ----------//
   	Set_LCD_REG(0x30, 0x0000); 	Set_LCD_REG(0x31, 0x0404);
   	Set_LCD_REG(0x32, 0x0003); 	Set_LCD_REG(0x35, 0x0405);
   	Set_LCD_REG(0x36, 0x0808); 	Set_LCD_REG(0x37, 0x0407);
   	Set_LCD_REG(0x38, 0x0303); 	Set_LCD_REG(0x39, 0x0707);
   	Set_LCD_REG(0x3C, 0x0504); 	Set_LCD_REG(0x3D, 0x0808);
   	//------------------ Set GRAM area ---------------//
   	Set_LCD_REG(0x61, 0x0001); // NDL,VLE, REV
   	Set_LCD_REG(0x6A, 0x0000); // set scrolling line
	//-------------- Partial Display Control ---------//
	Set_LCD_REG(0x80, 0x0000);	Set_LCD_REG(0x81, 0x0000);
	Set_LCD_REG(0x82, 0x0000);	Set_LCD_REG(0x83, 0x0000);
	Set_LCD_REG(0x84, 0x0000);	Set_LCD_REG(0x85, 0x0000);
	//-------------- Panel Control -------------------//
	Set_LCD_REG(0x90, 0x0010);
	Set_LCD_REG(0x92, 0x0000);
	Set_LCD_REG(0x07, 0x0133); // 262K color and display ON

	LCD_Draw_StartScreen();
}
#endif

#ifdef ILI9327
/******************************************************************************
* Function Name  : LCD_Init
* Description    : Init ILI9327 chip
******************************************************************************/
void LCD_Init(void)
{
	LCD_Reset();

   	LCD_Write_Com(0xE9);
	LCD_DATA(0x20);
	LCD_Write_Com(0x11);	//Exit Sleep
	delay_ms(100);
	LCD_Write_Com(0xD1);	//VCOM Control
	LCD_DATA(0x00);	LCD_DATA(0x71);	LCD_DATA(0x19);
	LCD_Write_Com(0xD0);	//Power_Setting
	LCD_DATA(0x07);	LCD_DATA(0x01);	LCD_DATA(0x08);

	LCD_Write_Com(0x3A);	//set_pixel_format
	LCD_DATA(0x05);			//16bit/pixel
	LCD_Write_Com(0xC1);	//Display_Timing_Setting for Normal/Partial Mode
	LCD_DATA(0x10);	LCD_DATA(0x10);	LCD_DATA(0x02);	LCD_DATA(0x02);
	LCD_Write_Com(0x36);	//output orientation
/*
	Bit B7 - Page Address Order :	0 = Top to Bottom	1 = Bottom to Top
	Bit B6 - Column Address Order:	0 = Left to Right	1 = Right to Left
	Bit B5 - Page/Column Order:		0 = Normal Mode		1 = Reverse Mode
	Bit B4 - Line Address Order:	0 = LCD Refresh Top to Bottom	1 = LCD Refresh Bottom to Top
	Bit B3 - RGB/BGR Order:			0 = RGB order		1 = BGR order
	Bit B2 - Display Data Latch Data Order:	This bit is set to 0. (Not supported)
	Bit B1 - Horizontal Flip:		0 = Normal display	1 = Flipped display
	Bit B0 - Vertical Flip:			0 = Normal display	1 = Flipped display
*/
	LCD_DATA(0b01001000);	//B6=1, BGR=1
	LCD_Write_Com(0xC0);	//Panel Driving Setting
	if (orientation)
		{LCD_DATA(0b00000101);}	//0,0,0, REV, SM, GS=1, BGR, SS = 1
	else
		{LCD_DATA(0b00000000);}	//0,0,0, REV, SM, GS = 0, BGR, SS=0
	LCD_DATA(0x35);	LCD_DATA(0x00);
	LCD_DATA(0x00);	LCD_DATA(0x01);	LCD_DATA(0x02);
	LCD_Write_Com(0xC5);	//Set frame rate
	LCD_DATA(0x04);
	LCD_Write_Com(0xD2);	//power setting
	LCD_DATA(0x01);	LCD_DATA(0x44);
	LCD_Write_Com(0xC8);	//Set Gamma
	LCD_DATA(0x04);	LCD_DATA(0x67);	LCD_DATA(0x35);	LCD_DATA(0x04);	LCD_DATA(0x08);
	LCD_DATA(0x06);	LCD_DATA(0x24);	LCD_DATA(0x01);	LCD_DATA(0x37);	LCD_DATA(0x40);
	LCD_DATA(0x03);	LCD_DATA(0x10);	LCD_DATA(0x08);	LCD_DATA(0x80);	LCD_DATA(0x00);
	LCD_Write_Com(0x29);	//display on
	LCD_Write_Com(0x2C);	//display on

	LCD_Draw_StartScreen();
}
#endif

#ifdef ILI9341
/******************************************************************************
* Function Name  : LCD_Init
* Description    : Init ILI9341 chip
******************************************************************************/
void LCD_Init(void)
{
	LCD_Reset();

  	LCD_Write_Com(0xCB);   	//POWER CONTROL A
   	LCD_DATA(0x39);   	LCD_DATA(0x2C);   	LCD_DATA(0x00);   	LCD_DATA(0x34);   	LCD_DATA(0x02);
   	LCD_Write_Com(0xCF);   	//POWER CONTROL B
   	LCD_DATA(0x00);   	LCD_DATA(0xC1);   	LCD_DATA(0x30);
   	LCD_Write_Com(0xE8);   	//DRIVER TIMING CONTROL A
   	LCD_DATA(0x85);   	LCD_DATA(0x00);   	LCD_DATA(0x78);
   	LCD_Write_Com(0xEA);   	//DRIVER TIMING CONTROL B
   	LCD_DATA(0x00);   	LCD_DATA(0x00);
   	LCD_Write_Com(0xED);   	//POWER ON SEQUENCE CONTROL
   	LCD_DATA(0x64);   	LCD_DATA(0x03);   	LCD_DATA(0x12);   	LCD_DATA(0x81);
   	LCD_Write_Com(0xF7);   	//PUMP RATIO CONTROL
   	LCD_DATA(0x20);
   	LCD_Write_Com(0xC0);   	//POWER CONTROL,VRH[5:0]
   	LCD_DATA(0x23);
   	LCD_Write_Com(0xC1);   	//POWER CONTROL,SAP[2:0];BT[3:0]
   	LCD_DATA(0x10);
   	LCD_Write_Com(0xC5);   	//VCM CONTROL
   	LCD_DATA(0x3E);   	LCD_DATA(0x28);
   	LCD_Write_Com(0xC7);   	//VCM CONTROL 2
   	LCD_DATA(0x86);
   	LCD_Write_Com(0x3A);   	//PIXEL FORMAT
   	LCD_DATA(0x55);	//16 bit color
   	LCD_Write_Com(0xB1);   	//FRAME RATIO CONTROL, STANDARD RGB COLOR
   	LCD_DATA(0x00);   	LCD_DATA(0x18);

   	LCD_Write_Com(0x36);	//MEMORY ACCESS CONTROL
   	//MY, MX, MV, ML, BGR, MH, x, x
   	LCD_DATA(0b00001000);	//MY = 0, MX = 0, MV = 0, BRG = 1

   	LCD_Write_Com(0xB6);	//DISPLAY FUNCTION CONTROL
   	LCD_DATA(0x08);
	if (orientation)
   		{LCD_DATA(0b11000010);}	//REV=1!, GS=1, SS=0, SM = 0, ISC=2
	else
   		{LCD_DATA(0b10100010);}	//REV=1, GS=0, SS=1, SM = 0, ISC=2
   	LCD_DATA(0x27);

   	LCD_Write_Com(0xF2);   	//3GAMMA FUNCTION DISABLE
   	LCD_DATA(0x00);
   	LCD_Write_Com(0x26);   	//GAMMA CURVE SELECTED
   	LCD_DATA(0x01);
   	LCD_Write_Com(0xE0);   	//POSITIVE GAMMA CORRECTION
   	LCD_DATA(0x0F);   	LCD_DATA(0x31);   	LCD_DATA(0x2B);   	LCD_DATA(0x0C);   	LCD_DATA(0x0E);
   	LCD_DATA(0x08);   	LCD_DATA(0x4E);   	LCD_DATA(0xF1);   	LCD_DATA(0x37);   	LCD_DATA(0x07);
   	LCD_DATA(0x10);   	LCD_DATA(0x03);   	LCD_DATA(0x0E);   	LCD_DATA(0x09);   	LCD_DATA(0x00);
   	LCD_Write_Com(0xE1);   	//NEGATIVE GAMMA CORRECTION
   	LCD_DATA(0x00);   	LCD_DATA(0x0E);   	LCD_DATA(0x14);   	LCD_DATA(0x03);   	LCD_DATA(0x11);
   	LCD_DATA(0x07);   	LCD_DATA(0x31);   	LCD_DATA(0xC1);   	LCD_DATA(0x48);   	LCD_DATA(0x08);
   	LCD_DATA(0x0F);   	LCD_DATA(0x0C);   	LCD_DATA(0x31);   	LCD_DATA(0x36);   	LCD_DATA(0x0F);
   	LCD_Write_Com(0x11);   	//EXIT SLEEP
   	delay_ms(100);
   	LCD_Write_Com(0x29);   	//TURN ON DISPLAY

	LCD_Draw_StartScreen();
}
#endif
