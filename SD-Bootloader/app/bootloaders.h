#ifndef __BOOTLOADERS_H
#define __BOOTLOADERS_H

#include "stm32f10x.h"

#include "typedefs.h"
#include "fat.h"
#include "flash.h"

static uc16 StatusShow[2] = {250, 292};
/*
static char *status[]=
{
	"The program file detection.",
	"To update.",
	"Updating program...",
	"Update success.",
	"Update failed.",
	"Please turn off the machine and unplug the SD card.",
};
*/
#define BOOTLOADER_BIN_ADDR         ( "0:/3D-GLCD.bin" )
#define BOOTLOADER_OLD_BIN_ADDR     ( "0:/3D-GLCD.old" )
#define BOOTLOADER_FLASH_ADDR       ( 0x08003800 )	//14k
#define BOOTLOADER_BUF_SIZE         ( 512 )
#define BOOTLOADER_SECT_SIZE        ( 8 )

FStat BootLoader_FromSDCard(void);
void IAP_LoadApp(u32 appxaddr);

#endif  //#ifndef __BOOTLOADERS_H
