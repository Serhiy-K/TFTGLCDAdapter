#include "init.h"
#include "LCD.h"
#include "protocol.h"

int main(void)
{
	__disable_irq();
	Global_Init();
	LCD_Init();
	__enable_irq();

	while (1)
	{
		if (New_cmd())
			handle_command();
	}
}
