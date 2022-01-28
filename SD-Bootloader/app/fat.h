#ifndef __FAT_H
#define __FAT_H
 
#include "stm32f10x.h"
#include <string.h>
#include "diskio.h"
#include "Sdcard_spi.h"
#include "ff.h"
#include "typedefs.h"

FStat FAT_Init(void);

#endif  //#ifndef __FAT_H

