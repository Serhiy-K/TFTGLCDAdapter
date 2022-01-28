#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"

#include <string.h>

#include "typedefs.h"
#include "util.h"

#define FLASH_PROG_TEST_EN          ( 1 )

#define FLASH_PAGE_SIZE             ( 0x400 )
#define FLASH_EMPTY                 ( 0xFFFFFFFF )

FStat Flash_Init(void);
FStat Flash_Write(uint32_t Addr, void *WriteBuf, uint32_t Len);
FStat Flash_Erase(uint32_t Addr, uint32_t Len);

#endif  //#ifndef __FLASH_H
