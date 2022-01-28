#include "init.h"
#ifdef BOOTLOADER
  #include "misc.h"
#endif
#include "protocol.h"

int main(void)
{
#ifdef BOOTLOADER
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x08003800);	//14kb offset for SD-bootloader
#endif
	__disable_irq();
	Global_Init();
	__enable_irq();

	while (1)
	{
		switch (New_cmd())
		{
			case INIT:			Init();		break;
			case CLR_SCREEN:	Clear_Screen();	break;
			case LCD_PUT:		Print_Lines();	break;
			case LCD_WRITE:		Out_Buffer();
		}
	}
}
