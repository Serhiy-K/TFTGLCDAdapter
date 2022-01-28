/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "stm32f10x.h"
#include "Sdcard_spi.h"

#define SD_CARD	 0
#define EX_FLASH 1

#define FLASH_SECTOR_SIZE 	512			  

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
  	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	u8 res=0;
		res = SD_Initialize();
		if (res)
		{
			SD_SPI_SpeedLow();
			SD_SPI_ReadWriteByte(0xff);
			SD_SPI_SpeedHigh();
		}
	if (res) return  STA_NOINIT;
	else return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	u8 res=0;

	if (drv != 0 || count == 0)	return RES_PARERR;

	res = SD_ReadDisk(buff,sector,count);
	if (res)
	{
		SD_SPI_SpeedLow();
		SD_SPI_ReadWriteByte(0xff);
		SD_SPI_SpeedHigh();
	}

	if (res == 0x00) return RES_OK;
	else return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	u8 res=0;

	if (drv != 0 || count == 0)
		return RES_PARERR;

	res = SD_WriteDisk((u8*)buff, sector, count);

	if (res == 0x00)	return RES_OK;
	else				return RES_ERROR;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	if (drv != 0)	return RES_OK;

	switch(ctrl)
	{
		case CTRL_SYNC:
				SPI_PORT->BRR = SPI_CS;
			if (SD_WaitReady() == 0)	res = RES_OK;
			else						res = RES_ERROR;
			SPI_PORT->BSRR = SPI_CS;
			break;	 
		case GET_SECTOR_SIZE:
			*(WORD*)buff = 512;
			res = RES_OK;
			break;
		case GET_BLOCK_SIZE:
			*(WORD*)buff = 8;
			res = RES_OK;
			break;	 
		case GET_SECTOR_COUNT:
			*(DWORD*)buff = SD_GetSectorCount();
			res = RES_OK;
			break;
		default:
			res = RES_PARERR;
			break;
	}
	return res;
}
