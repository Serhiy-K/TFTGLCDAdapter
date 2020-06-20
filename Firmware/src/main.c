#include "init.h"
#include "protocol.h"

int main(void)
{
	__disable_irq();
	Global_Init();
	__enable_irq();

	while (New_cmd() != INIT) {}

	while (1)
	{
		if (New_cmd() == INIT)
			Command_Handler();
		out_buffer();
	}
}
