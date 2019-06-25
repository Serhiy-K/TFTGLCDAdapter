#ifndef _PIC_H_
#define _PIC_H_

#include "defines.h"

extern const uint16_t heat_48x48[2304];
extern const uint16_t fan_48x48[2304];
extern const uint16_t bed_48x48[2304];
#ifdef ONE_HOTEND
extern const uint16_t extrude_48x48[2304];
#else
extern const uint16_t extrude1_48x48[2304];
extern const uint16_t extrude2_48x48[2304];
extern const uint16_t extrude3_48x48[2304];
#endif

#endif
