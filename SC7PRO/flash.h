#ifndef __FLASH_H__
#define __FLASH_H__

#define STM32G0_FLASH_SIZE 256*1024
#define FLASH_BASE 0x8000000

long download_to_flash(unsigned long offset);
int check_firmware(unsigned long size);

#endif
