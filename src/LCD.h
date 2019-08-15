#ifndef _LCD_H_
#define _LCD_H_

#include "stm32f10x.h"
#include "defines.h"


#define RES_LCD_set	LCD_CTRL_PORT->BSRR = LCD_RST;
#define RES_LCD_clr	LCD_CTRL_PORT->BSRR = LCD_RST << 16;

#define RS_LCD_set	LCD_CTRL_PORT->BSRR = LCD_RS;
#define RS_LCD_clr	LCD_CTRL_PORT->BSRR = LCD_RS << 16;

#define WR_LCD_set	LCD_CTRL_PORT->BSRR = LCD_WR;
#define WR_LCD_clr	LCD_CTRL_PORT->BSRR = LCD_WR << 16;

#define CS_LCD_set	LCD_CTRL_PORT->BSRR = LCD_CS;
#define CS_LCD_clr	LCD_CTRL_PORT->BSRR = LCD_CS << 16;

#define LCD_DATA(data)	LCD_DATA_PORT->BRR = LCD_DATA_MASK; LCD_DATA_PORT->BSRR = data; WR_LCD_clr; WR_LCD_set;

// colors
#define White       		0xffff
#define Black       		0x0000
#define LightBlack  		0x0841
#define LightBlack2  		0x3186
#define Gray        		0x8c71
#define LightGray   		0xce59
#define LightGray2			0xbdf7
#define LightGray3          0x94b2
#define LightGray4          0x8410
#define LightSky    		0x7e7f
#define Red         		0xf800
#define DarkRed     		0x5000
#define LightRed    		0xfcb2
#define LightRed2           0xf904
#define LightGreen  		0x0679
#define Blue        		0x001f
#define LightBlue1  		0x03bd
#define LBlue2  			0x36bf
#define LighGreen   		0x87f0
#define Green1      		0x0160
#define Orange      		0xeba0
#define Orange1     		0xfb23
#define DarkOrange  		0xa9a0
#define DarkOrange2			0x8960
#define Auqa				0x332c
#define DarkAuqa			0x01e4
#define GrayBlue    		0x041f
#define Yellow          	0xffe0
#define StillBlue     		0x10a4

#define BackColor	Black

extern const char Courier_New_Bold_16x24[256][48];
extern const char LiberationMono_16x24[256][48];

void LCD_SetArea(uint16_t X0pos, uint16_t Y0pos, uint16_t X1pos, uint16_t Y1pos);
void LCD_ClearArea(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Color);
void LCD_FillScreen(uint16_t Color);
void LCD_Draw_Picture(uint16_t X0pos, uint16_t Y0pos, const uint16_t *pic);
void LCD_Clear_Picture(uint16_t X0pos, uint16_t Y0pos);

void LCD_Set_TextColor(uint16_t front, uint16_t back);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_DrawChar(char c);
void LCD_DrawSymbol(const uint8_t *ptr);
void LCD_DrawChar_XY(uint16_t XPos, uint16_t YPos, char c);
void LCD_PutStrig(char *str);
void LCD_PutStrig_XY(uint16_t XPos, uint16_t YPos, char *str);

void LCD_Init(void);

#endif
