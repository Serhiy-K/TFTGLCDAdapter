#include "bootloaders.h"
#include "delay.h"

static FIL Fsrc;
static uint8_t ReadBuf[BOOTLOADER_BUF_SIZE];

void IAP_LoadApp()
{
	BOOT_PORT->BRR = BOOT_PIN;

	typedef void (*IAP_FUNCTION)(void);
	IAP_FUNCTION  jump2app;

	jump2app = (IAP_FUNCTION)*(vu32*)(MAIN_PROGRAM_START_ADDR + 4);
	__set_MSP(*(vu32*)MAIN_PROGRAM_START_ADDR);
	jump2app();
}

FStat BootLoader_FromSDCard(void)
{
	FRESULT res;
	FStat stat;
	UINT cnt;
	UINT addr;


	res = f_open( &Fsrc, MAIN_PROGRAM_FILE_BIN, FA_READ | FA_OPEN_EXISTING );
	if ( res == FR_OK )
	{
    	FLASH_Unlock();
		delay_ms(300);
		addr = MAIN_PROGRAM_START_ADDR;
		res = f_read( &Fsrc, ReadBuf, BOOTLOADER_BUF_SIZE, &cnt );
		if ( FR_OK != res )	return FuncErr;
		if ( cnt < BOOTLOADER_SECT_SIZE )	return FuncErr;
		while ( cnt )
		{
			stat = Flash_Write( addr, ReadBuf, cnt );
			if ( FuncOK != stat )	return FuncErr;
			addr += cnt;
			res = f_read( &Fsrc, ReadBuf, BOOTLOADER_BUF_SIZE, &cnt );
			if ( FR_OK != res )	return FuncErr;
		}
		FLASH_Lock();
		f_close( &Fsrc );
		f_unlink(MAIN_PROGRAM_FILE_BIN);	//remove file
	}
	return FuncOK;
}
