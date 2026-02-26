#ifndef __FLASH_H__
#define __FLASH_H__

void flash_program_page(unsigned long addr, void *data, unsigned int len);
void flash_clear_error(void);
//long download_to_flash(unsigned long offset);
// int check_firmware(unsigned long size);
#define GD32E50X_FLASH_SIZE (256*1024)

#endif
