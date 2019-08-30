/******************************************************************************
 Graphics Library, 8-bit parallel data bus
******************************************************************************/
#include "LCD.h"
#include "Pictures.h"
#include "systick.h"

volatile uint16_t TCH, TCL;
volatile uint16_t BCH, BCL;
uint16_t Xcour;
extern uint8_t	protocol;

#ifdef ILI9325
/******************************************************************************
 Description    : Write data to LCD reg
 Input          : RegisterIndex, data
******************************************************************************/
static __inline void Set_LCD_REG(uint8_t RegisterIndex, uint16_t Data)
{
	RS_LCD_clr;
	LCD_DATA(0x00);	LCD_DATA(RegisterIndex);
	RS_LCD_set;

	LCD_DATA(Data >> 8);	LCD_DATA(Data & 0x00ff);
}
#endif
#ifdef ILI9327
/******************************************************************************
 Description    : Write data to LCD reg
 Input          : RegisterIndex, data
******************************************************************************/
static __inline void Lcd_Write_Com(uint8_t RegisterIndex)
{
	RS_LCD_clr;
	LCD_DATA(RegisterIndex);
	RS_LCD_set;
}
#endif
/******************************************************************************
 Description    : Fill screen with one color
 Input          : Color
******************************************************************************/
void LCD_FillScreen(uint16_t Color)
{
    uint32_t cntT = 0;
	uint16_t CH = Color >> 8;
	uint16_t CL = Color & 0x00ff;
    LCD_SetArea(0, 0, LCDXMAX - 1, LCDYMAX - 1);
    do
    {
    	LCD_DATA(CH);	LCD_DATA(CL);
    }
    while (++cntT < (LCDXMAX * LCDYMAX));
    CS_LCD_set;
}
/******************************************************************************
 Description    : Fill screen area with one color
 Input          : X0pos, Y0pos, X1pos, Y1pos, Color
******************************************************************************/
void LCD_ClearArea(uint16_t X0pos, uint16_t Y0pos, uint16_t X1pos, uint16_t Y1pos, uint16_t Color)
{
	uint32_t pntNum;
	uint16_t CH = Color >> 8;
	uint16_t CL = Color & 0x00ff;

   // calculate dot numbers
   pntNum = ((X1pos - X0pos) + 1) * ((Y1pos - Y0pos) + 1);

   LCD_SetArea(X0pos, Y0pos, X1pos, Y1pos);
   do
   {
	   LCD_DATA(CH);	LCD_DATA(CL);
   }
   while (pntNum--);
   CS_LCD_set;
}
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
	Lcd_Write_Com(0x2A);
	LCD_DATA(Y0 >> 8);
	LCD_DATA(Y0);
	LCD_DATA(Y1 >> 8);
	LCD_DATA(Y1);
	Lcd_Write_Com(0x2B);
#ifdef LANDSCAPE_L
	LCD_DATA(X0 >> 8);
	LCD_DATA(X0);
	LCD_DATA(X1 >> 8);
	LCD_DATA(X1);
#else
	LCD_DATA(X0 >> 8);
	LCD_DATA(X0);
	LCD_DATA(X1 >> 8);
	LCD_DATA(X1);
#endif
#endif	//ILI9327

	RS_LCD_clr;
#ifdef ILI9325
	LCD_DATA(0x00);	LCD_DATA(0x22);	//lcd_Draw_Start
#endif
#ifdef ILI9327
	LCD_DATA(0x2C);	//lcd_Draw_Start
#endif
	RS_LCD_set;
}
/******************************************************************************
 Description    : Draw picture 48õ48
 Input          : X0pos, Y0pos, *pic
******************************************************************************/
void LCD_Draw_Picture(uint16_t X0pos, uint16_t Y0pos, const char *pic)
{
	uint16_t i;

	LCD_SetArea(X0pos, Y0pos, X0pos + 47, Y0pos + 47 );
	for (i = 0; i < 2304; i++)
	{
		LCD_DATA(colors[*pic - 'A'] >> 8);	LCD_DATA(colors[*pic - 'A'] & 0x00ff);
		pic++;
	}
	CS_LCD_set;
}
/******************************************************************************
 Description    : Erase picture 48õ48
 Input          : X0pos, Y0pos
******************************************************************************/
void LCD_Clear_Picture(uint16_t X0pos, uint16_t Y0pos)
{
	uint16_t i;

	uint16_t CH = (BackColor >> 8);
	uint16_t CL = (BackColor & 0x00ff);

	LCD_SetArea(X0pos, Y0pos, X0pos + 47, Y0pos + 47 );
	for(i = 0; i < 2304; i++)
	{
		LCD_DATA(CH);	LCD_DATA(CL);
	}
	CS_LCD_set;
}
/******************************************************************************
 Set color for text and back
******************************************************************************/
void LCD_Set_TextColor(uint16_t front, uint16_t back)
{
	TCH = front >> 8;
	TCL = front & 0x00ff;
	BCH = back >> 8;
	BCL = back & 0x00ff;
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
/******************************************************************************
 Description    : Draw char to current position
 Input          : char c
******************************************************************************/
void LCD_DrawChar(char c)
{
	uint8_t i, j;
	const char *ptr; // pointer to char data
	uint8_t mask = 0x80;  // 0b10000000

	if (protocol == Marlin1)
		c = HD44780_to_ASCII(c);
	ptr = &FONT[(uint8_t)c][0];
	for(i = 0; i < CHAR_BYTES; i++)
	{
		for(j = 0; j < 8; j++)
		{
			if(*ptr & mask) {LCD_DATA(TCH);	LCD_DATA(TCL);}
			else 			{LCD_DATA(BCH);	LCD_DATA(BCL);}
			mask = mask >> 1;
		}
		mask = 0x80;
		ptr++;
	}
	Xcour++;	//increment cursor position
}
/******************************************************************************
 Description    : Draw symbol to current char position
 Input          : pointer to 16x24 symbol table
******************************************************************************/
void LCD_DrawSymbol(const uint8_t *ptr)
{
	uint8_t i, j;
	uint8_t mask = 0x80;  // 0b10000000

	for(i = 0; i < CHAR_BYTES; i++)
	{
		for(j = 0; j < 8; j++)
		{
			if(*ptr & mask) {LCD_DATA(TCH);	LCD_DATA(TCL);}
			else 			{LCD_DATA(BCH);	LCD_DATA(BCL);}
			mask = mask >> 1;
		}
		mask = 0x80;
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
	while(*str != 0)
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
#ifdef ILI9325
/******************************************************************************
* Function Name  : LCD_Init
* Description    : Init ILI9325 chip
******************************************************************************/
void LCD_Init(void)
{
	CS_LCD_set;
	WR_LCD_set;
	RS_LCD_set;

	/* Reset chip */
   	RES_LCD_clr;
   	delay_ms(10);
   	RES_LCD_set;
   	delay_ms(50);
   	CS_LCD_clr;
   	delay_ms(100);			/* Wait Stability */

   	//for 8-bit interface
   	RS_LCD_clr;
   	LCD_DATA(0x00);	LCD_DATA(0x00);	LCD_DATA(0x00);	LCD_DATA(0x00);
   	RS_LCD_set

   	Set_LCD_REG(0x02, 0x0200); // set 1 line inversion
#ifdef	LANDSCAPE_L
   	Set_LCD_REG(0x01, 0x0100); // set SS=1 and SM=0 bit
   	Set_LCD_REG(0x60, 0x2700); // set GS=0 bit
#else
   	Set_LCD_REG(0x01, 0x0000); // set SS=0 and SM=0 bit
   	Set_LCD_REG(0x60, 0xA700); // set GS bit
#endif
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
   	Set_LCD_REG(0x30, 0x0000);
   	Set_LCD_REG(0x31, 0x0404);
   	Set_LCD_REG(0x32, 0x0003);
   	Set_LCD_REG(0x35, 0x0405);
   	Set_LCD_REG(0x36, 0x0808);
   	Set_LCD_REG(0x37, 0x0407);
   	Set_LCD_REG(0x38, 0x0303);
   	Set_LCD_REG(0x39, 0x0707);
   	Set_LCD_REG(0x3C, 0x0504);
   	Set_LCD_REG(0x3D, 0x0808);
   	//------------------ Set GRAM area ---------------//
   	Set_LCD_REG(0x61, 0x0001); // NDL,VLE, REV
   	Set_LCD_REG(0x6A, 0x0000); // set scrolling line
	//-------------- Partial Display Control ---------//
	Set_LCD_REG(0x80, 0x0000);
	Set_LCD_REG(0x81, 0x0000);
	Set_LCD_REG(0x82, 0x0000);
	Set_LCD_REG(0x83, 0x0000);
	Set_LCD_REG(0x84, 0x0000);
	Set_LCD_REG(0x85, 0x0000);
	//-------------- Panel Control -------------------//
	Set_LCD_REG(0x90, 0x0010);
	Set_LCD_REG(0x92, 0x0000);
	Set_LCD_REG(0x07, 0x0133); // 262K color and display ON

	CS_LCD_set;

	LCD_FillScreen(BackColor);
	LCD_Set_TextColor(Yellow, Blue);
	LCD_PutStrig_XY(0, 0, "                    ");
	LCD_PutStrig_XY(0, 1, "  TFT GLCD Adapter  ");
	LCD_PutStrig_XY(0, 2, "                    ");
	LCD_Set_TextColor(White, BackColor);
	LCD_PutStrig_XY(0, 4, "Waiting for printer ");
	LCD_PutStrig_XY(4, 5,     "connection..");
}
#endif
#ifdef ILI9327
/******************************************************************************
* Function Name  : LCD_Init
* Description    : Init ILI9327 chip
******************************************************************************/
void LCD_Init(void)
{
	CS_LCD_set;
	WR_LCD_set;
	RS_LCD_set;

	/* Reset chip */
   	RES_LCD_clr;
   	delay_ms(10);
   	RES_LCD_set;
   	delay_ms(50);
   	CS_LCD_clr;
   	delay_ms(100);			/* Wait Stability */

   	//for 8-bit interface
   	RS_LCD_clr;
   	LCD_DATA(0x00);	LCD_DATA(0x00);	LCD_DATA(0x00);	LCD_DATA(0x00);
   	RS_LCD_set

   	Lcd_Write_Com(0xE9);
	LCD_DATA(0x20);
	Lcd_Write_Com(0x11);	//Exit Sleep
	delay_ms(100);
	Lcd_Write_Com(0xD1);	//VCOM Control
	LCD_DATA(0x00);	LCD_DATA(0x71);	LCD_DATA(0x19);
	Lcd_Write_Com(0xD0);	//Power_Setting
	LCD_DATA(0x07);	LCD_DATA(0x01);	LCD_DATA(0x08);

	Lcd_Write_Com(0x3A);	//set_pixel_format
	LCD_DATA(0x05);			//16bit/pixel
	Lcd_Write_Com(0xC1);	//Display_Timing_Setting for Normal/Partial Mode
	LCD_DATA(0x10);	LCD_DATA(0x10);	LCD_DATA(0x02);	LCD_DATA(0x02);
	Lcd_Write_Com(0x36);	//output orientation
/*
	Bit B7 – Page Address Order :	0 = Top to Bottom	1 = Bottom to Top
	Bit B6 – Column Address Order:	0 = Left to Right	1 = Right to Left
	Bit B5 – Page/Column Order:		0 = Normal Mode		1 = Reverse Mode
	Bit B4 –Line Address Order:		0 = LCD Refresh Top to Bottom	1 = LCD Refresh Bottom to Top
	Bit B3 – RGB/BGR Order:			0 = RGB order		1 = BGR order
	Bit B2 –Display Data Latch Data Order:	This bit is set to ‘0’. (Not supported)
	Bit B1 – Horizontal Flip:		0 = Normal display	1 = Flipped display
	Bit B0 – Vertical Flip:			0 = Normal display	1 = Flipped display
*/
	LCD_DATA(0b01001000);	//B6=1, BGR=1
	Lcd_Write_Com(0xC0);	//Panel Driving Setting
#ifdef	LANDSCAPE_L
	LCD_DATA(0b00000000);	//0,0,0, REV, SM, GS = 0, BGR, SS=0
#else
	LCD_DATA(0b00000101);	//0,0,0, REV, SM, GS=1, BGR, SS = 1
#endif
	LCD_DATA(0x35);	LCD_DATA(0x00);
	LCD_DATA(0x00);	LCD_DATA(0x01);	LCD_DATA(0x02);
	Lcd_Write_Com(0xC5);	//Set frame rate
	LCD_DATA(0x04);
	Lcd_Write_Com(0xD2);	//power setting
	LCD_DATA(0x01);	LCD_DATA(0x44);
	Lcd_Write_Com(0xC8);	//Set Gamma
	LCD_DATA(0x04);	LCD_DATA(0x67);	LCD_DATA(0x35);	LCD_DATA(0x04);	LCD_DATA(0x08);
	LCD_DATA(0x06);	LCD_DATA(0x24);	LCD_DATA(0x01);	LCD_DATA(0x37);	LCD_DATA(0x40);
	LCD_DATA(0x03);	LCD_DATA(0x10);	LCD_DATA(0x08);	LCD_DATA(0x80);	LCD_DATA(0x00);
	Lcd_Write_Com(0x29);	//display on
	Lcd_Write_Com(0x2C);	//display on

	CS_LCD_set;

	LCD_FillScreen(BackColor);
	LCD_Set_TextColor(Yellow, Blue);
	LCD_PutStrig_XY(0, 0, "                         ");
	LCD_PutStrig_XY(0, 1, "    TFT GLCD Adapter     ");
	LCD_PutStrig_XY(0, 2, "                         ");
	LCD_Set_TextColor(White, BackColor);
	LCD_PutStrig_XY(0, 4, "   Waiting for printer   ");
	LCD_PutStrig_XY(6, 5,       "connection...");
}
#endif
