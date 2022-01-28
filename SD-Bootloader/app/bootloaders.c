#include "bootloaders.h"
#include "delay.h"

static FIL Fsrc;
static uint8_t ReadBuf[BOOTLOADER_BUF_SIZE];

typedef void (*IAP_FUNCTION)(void);

void IAP_LoadApp(u32 appxAddr)
{
	BOOT_PORT->BRR = BOOT_PIN;

	IAP_FUNCTION  jump2app;

	jump2app = (IAP_FUNCTION)*(vu32*)(appxAddr + 4);
	__set_MSP(*(vu32*)appxAddr);
	jump2app();
}

FStat BootLoader_FromSDCard(void)
{
	FRESULT res;
	FStat stat;
	UINT cnt;
	UINT addr;

	res = f_open( &Fsrc, BOOTLOADER_BIN_ADDR, FA_READ | FA_OPEN_EXISTING );
	if ( res == FR_OK )
	{	//Loading firmware
		delay_ms(300);
		addr = BOOTLOADER_FLASH_ADDR;
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
		f_close( &Fsrc );
		f_unlink(BOOTLOADER_BIN_ADDR);	//remove file
	}
	return FuncOK;
}
