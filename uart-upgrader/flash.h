#ifndef __FLASH_H__
#define __FLASH_H__

long download_to_flash(unsigned long offset);
int check_firmware(unsigned long size);

#endif
