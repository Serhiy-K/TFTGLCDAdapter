#include "init.h"
#include "protocol.h"

int main(void)
{
	__disable_irq();
	Global_Init();
	__enable_irq();

	while (1)
	{
		switch (New_cmd())
		{
			case INIT:			Init();		break;
			case CLR_SCREEN:	Clear_Screen();	break;
			case LCD_PUT:		Print_1_Line();	break;
			case LCD_WRITE:		Out_Buffer();
		}
	}
}
