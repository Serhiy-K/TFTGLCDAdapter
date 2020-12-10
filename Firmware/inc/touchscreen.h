#ifndef _TOCHSCREEN_H_
#define _TOCHSCREEN_H_

uint8_t screen_mode;
uint8_t line_3[CHARS_PER_LINE];
uint8_t line_2[CHARS_PER_LINE];
uint8_t line_1[CHARS_PER_LINE];
uint8_t cur_line;
uint8_t delay_cnt;

enum screen_modes {MAIN_SCREEN = 0, MENU_SCREEN, SELECT_SCREEN, EDIT_SCREEN, MAIN_UBL, EDIT_UBL, START_SCREEN};
#define ADC_X_middle 0x07ff

void Read_Touch();

#endif