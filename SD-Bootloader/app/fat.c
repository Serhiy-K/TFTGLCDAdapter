#include "fat.h"

FATFS FileSystem;

/*
 *  FUNCTION    :   FAT_Init
 *  DESCRIPTION :   FAT file sysetm init , mount SD card
 *  INPUT       :   no
 *  OUTPUT      :   succeed:FuncOK fail:FuncErr
 */
FStat FAT_Init(void)
{
    DSTATUS stat;
    FRESULT res;

    BOOT_PORT->BSRR = BOOT_PIN;

    stat = disk_initialize(0);
    if (RES_OK != stat)
        return FuncErr; //Initialize disk fail

    res = f_mount(0, &FileSystem);
    if (FR_OK != res)
        return FuncErr; //Mount sdcard fail

    return FuncOK;
}
