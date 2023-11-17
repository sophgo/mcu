#ifndef __FLASH_H__
#define __FLASH_H__

void flash_program_page(unsigned long addr, void *data, unsigned int len);
void flash_clear_error(void);

#endif
