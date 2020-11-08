#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "defines.h"

enum Commands {
	GET_SPI_DATA = 0,	// is a read results of previous command
	READ_BUTTONS,
	READ_ENCODER,
	LCD_WRITE,			// write all screen
	BUZZER,
	BRIGHTNES,			//CONTRAST command in Smoothieware
	// Other commands... 0xE0 thru 0xFF
	GET_LCD_ROW = 0xE0,
	GET_LCD_COL,
	LCD_PUT,			// write one line to screen
	CLR_SCREEN,			// for Marlin
	INIT = 0xFE			// Initialize
};

uint8_t New_cmd();
void Clear_Screen();
void Init();
void Out_Buffer();
void Print_1_Line();

#endif
