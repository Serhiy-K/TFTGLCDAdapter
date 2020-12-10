#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "defines.h"

#ifdef HW_VER_3

#define SETTINGSVERSION 1

struct Settings {
	uint16_t version;
	uint16_t adc_Ymin;
	uint16_t adc_Ymax;
	uint16_t adc_1_line;
} Settings;

void saveSettings();
void restoreSettings();

#endif

#endif /* SETTINGS_H_ */