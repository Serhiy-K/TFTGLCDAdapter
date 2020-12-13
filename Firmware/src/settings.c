#include "settings.h"

#ifdef HW_VER_3

#include "stm32f10x_flash.h"

#define FLASH_ADDR (0x8000000 | 64512)	//Flash start OR'ed with the maximum amount of flash - 1024 bytes

void saveSettings() {
	FLASH_Unlock();
	FLASH_ErasePage(FLASH_ADDR);

	uint16_t *data = (uint16_t *)&Settings;

	for (uint16_t i = 0; i < (sizeof(Settings) / 2); i++)
		FLASH_ProgramHalfWord(FLASH_ADDR + (i * 2), data[i]);
	FLASH->CR |= FLASH_CR_LOCK;
}
//----------------------------------------------------------------------------
void resetSettings() {
	Settings.version = SETTINGSVERSION;
	Settings.adc_Ymin = 0;
	Settings.adc_Ymax = 0x0fff;
	Settings.adc_1_line = 409;	//=(adc_Ymax-adc_Ymin)/TEXT_LINES
}
//----------------------------------------------------------------------------
void restoreSettings() {
	uint16_t *data = (uint16_t*) &Settings;
	for (uint8_t i = 0; i < (sizeof(Settings) / 2); i++)
		data[i] = *(uint16_t *) (FLASH_ADDR + (i * 2));
	if (Settings.version != SETTINGSVERSION)
	{
		resetSettings();	saveSettings();
	}

}

#endif