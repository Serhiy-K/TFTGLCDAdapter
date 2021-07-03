#ifndef _PIC_H_
#define _PIC_H_

#include "defines.h"

extern const uint16_t colors[26];
//for 3D-printer
extern const char heat_48x48[2304];
extern const char fan_48x48[2304];
extern const char bed_48x48[2304];
extern const char extrude_48x48[2304];
extern const char extrude1_48x48[2304];
extern const char extrude2_48x48[2304];
extern const char extrude3_48x48[2304];

extern const char extrude_off_48x48[2304];
extern const char bed_off_48x48[2304];
extern const char fan_off_48x48[2304];

//for laser
extern const char cool_48x48[2304];
extern const char cool_off_48x48[2304];
extern const char flow_48x48[2304];
extern const char flow_off_48x48[2304];
extern const char amp_48x48[2304];
extern const char amp_off_48x48[2304];

#endif
