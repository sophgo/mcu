/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <sdcard.h>
#include <debug.h>
#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
// #define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_SD		0	/* Example: Map MMC/SD card to physical drive 1 */
// #define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#define IO_BUFFER_SIZE	4096
#define SD_BLK_SIZE	512

static int SD_disk_status(void)
{
	return 0;
}

static int SD_disk_initialize(void)
{
	assert((FF_MIN_SS == FF_MAX_SS) &&
		(FF_MIN_SS == SD_BLK_SIZE));
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	 BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_SD :
		result = SD_disk_status();

		// translate the reslut code here
		stat = result;

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_SD :
		result = SD_disk_initialize();

		// translate the reslut code here
		stat = result;

		return stat;
	}
	return STA_NOINIT;
}

static UINT sd_disk_read(BYTE *buff, DWORD sector, UINT count)
{

	int err;
	UINT i;

	for (i = 0; i < count; ++i) {
		err = sd_block_read((void *)(((uint8_t *)buff) + i * SD_BLK_SIZE), (sector + i) * SD_BLK_SIZE, SD_BLK_SIZE);
		if (err != SD_OK) {
			debug("error\n");
			return -1;
		}
	}
	return count;

}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	UINT result;

	switch (pdrv) {
	case DEV_SD :
		// translate the arguments here

		result = sd_disk_read(buff, sector, count);

		// translate the reslut code here
		if (result == count)
			res = RES_OK;
		else
			res = RES_ERROR;

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;
	uint32_t *pwritebuf = (uint32_t *)buff;
	switch (pdrv) {
	case DEV_SD :
		// translate the arguments here

		result = sd_multiblocks_write(pwritebuf, sector, SD_BLK_SIZE, count);

		// translate the reslut code here

		if (result == count)
			res = RES_OK;
		else
			res = RES_ERROR;

		return res;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_SD :

		// Process of the command for the MMC/SD card
		res = result = 0;

		return res;
	}

	return RES_PARERR;
}

