#ifndef _LCD_H_
#define _LCD_H_

#include "stm32f10x.h"
#include "defines.h"


#define RES_LCD_set     LCD_CTRL_PORT->BSRR = LCD_RST;
#define RES_LCD_clr     LCD_CTRL_PORT->BRR = LCD_RST;

#define RS_LCD_set      LCD_CTRL_PORT->BSRR = LCD_RS;
#define RS_LCD_clr      LCD_CTRL_PORT->BRR = LCD_RS;

#define RD_LCD_set      LCD_CTRL_PORT->BSRR = LCD_RD;
#define RD_LCD_clr      LCD_CTRL_PORT->BRR = LCD_RD;

#define WR_LCD_set      LCD_CTRL_PORT->BSRR = LCD_WR;
#define WR_LCD_clr      LCD_CTRL_PORT->BRR = LCD_WR;

#define H_WR_LCD_set	LCD_H_PORT->BSRR = LCD_H_WR;
#define H_WR_LCD_clr	LCD_H_PORT->BRR = LCD_H_WR;

#define CS_LCD_set      LCD_CTRL_PORT->BSRR = LCD_CS;
#define CS_LCD_clr      LCD_CTRL_PORT->BRR = LCD_CS;

#define WR_Puls         WR_LCD_clr; WR_LCD_set;
#define H_WR_Puls       H_WR_LCD_set; H_WR_LCD_clr;

#ifndef HW_VER_2
 #define LCD_DATA(data)     LCD_DATA_PORT->BRR = LCD_DATA_MASK; LCD_DATA_PORT->BSRR = data; WR_Puls;
 #define LCD_DATA_00        LCD_DATA_PORT->BRR = LCD_DATA_MASK; WR_Puls; WR_Puls;
 #define LCD_DATA_FF        LCD_DATA_PORT->BSRR = LCD_DATA_MASK; WR_Puls; WR_Puls;
#else
 #define LCD_DATA(data)     LCD_DATA_PORT->ODR = (uint16_t)data; WR_Puls;
#endif

// colors
#define Black               0x0000
#define LightBlack          0x0841
#define LightBlack2         0x3186
#define Gray                0x8c71
#define LightGray           0xce59
#define LightGray2          0xbdf7
#define LightGray3          0x94b2
#define LightGray4          0x8410
#define White               0xffff
#define DarkRed             0x5000
#define Red                 0xf800
#define LightRed            0xfcb2
#define LightRed2           0xf904
#define Green               0x0160
#define LightGreen          0x0679
#define LightGreen2         0x87f0
#define Blue                0x001f
#define LightBlue           0x03bd
#define LightBlue2          0x36bf
#define LightSky            0x7e7f
#define Orange              0xeba0
#define Orange1             0xfb23
#define DarkOrange          0xa9a0
#define DarkOrange2         0x8960
#define Auqa                0x332c
#define DarkAuqa            0x01e4
#define GrayBlue            0x041f
#define Yellow              0xffe0
#define StillBlue           0x10a4

#define BackColor	Black

extern const char FONT[256][48];

void LCD_ClearScreen();
void LCD_FillRect(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Color);
void LCD_Draw_Picture(uint16_t X0pos, uint16_t Y0pos, const char *pic);
void LCD_Clear_Picture(uint16_t X0pos, uint16_t Y0pos);

void LCD_Set_TextColor(uint16_t front, uint16_t back);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_DrawChar(char c);
void LCD_DrawChar_XY(uint16_t XPos, uint16_t YPos, char c);
void LCD_PutStrig(char *str);
void LCD_PutStrig_XY(uint16_t XPos, uint16_t YPos, char *str);

void LCD_Init(void);

#endif  //_LCD_H_
