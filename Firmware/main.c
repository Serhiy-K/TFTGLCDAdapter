#include "init.h"
#include "protocol.h"

int main(void)
{
	__disable_irq();
	Global_Init();
	__enable_irq();

	while (1)
	{
		if (New_cmd())
			Command_Handler();
	}
}
