#include "LCD.h"
#include "protocol.h"

void print_dec(int16_t dec)
{
	int16_t t;
	if (dec < 0)    {LCD_DrawChar('-');	dec = -dec;}
    else             LCD_DrawChar(' ');
	if (dec > 10000) {t = dec / 10000;	LCD_DrawChar('0' + t);	dec -= t * 10000;}
	if (dec > 1000)	{t = dec / 1000;	LCD_DrawChar('0' + t);	dec -= t * 1000;}
	if (dec > 100)	{t = dec / 100;		LCD_DrawChar('0' + t);	dec -= t * 100;}
	if (dec > 10)	{t = dec / 10;		LCD_DrawChar('0' + t);	dec -= t * 10;}
	LCD_DrawChar('0' + dec);
}
//----------------------------------------------------------------------------
void test_beep()
{
	duration[buzcnt] = 50;
	freq[buzcnt++] = 2000;
	Buzzer();
}
