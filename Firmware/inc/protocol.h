#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "defines.h"

enum Commands {
	GET_SPI_DATA = 0,	// is a read results of previous command
	READ_BUTTONS,
	READ_ENCODER,
	LCD_WRITE,			// write data into buffer and output to screen
	BUZZER,
	BRIGHTNES,			//CONTRAST command in Smoothieware
	// Other commands... 0xE0 thru 0xFF
	GET_LCD_ROW = 0xE0,
	GET_LCD_COL,
	LCD_PUT,			// only write data into buffer
	INIT = 0xFE			// Initialize
};

uint8_t New_cmd();
void Command_Handler();
void out_buffer();

#endif
