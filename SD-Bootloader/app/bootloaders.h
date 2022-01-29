#ifndef __BOOTLOADERS_H
#define __BOOTLOADERS_H

#include "stm32f10x.h"

#include "typedefs.h"
#include "fat.h"
#include "flash.h"

static uc16 StatusShow[2] = {250, 292};

#define BOOTLOADER_BUF_SIZE         ( 512 )
#define BOOTLOADER_SECT_SIZE        ( 8 )

#define MAIN_PROGRAM_START_ADDR     ( 0x08000000 + 1024 * 14)	//14kB
#define MAIN_PROGRAM_FILE_BIN       ( "0:/3D-GLCD.bin" )

FStat BootLoader_FromSDCard(void);
void IAP_LoadApp();

#endif  //#ifndef __BOOTLOADERS_H
