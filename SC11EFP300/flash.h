#ifndef __FLASH_H__
#define __FLASH_H__

void flash_program_page(unsigned long addr, void *data, unsigned int len);
void flash_clear_error(void);
uint8_t eeprom_read_byte(uint32_t offset);
void eeprom_write_byte(uint32_t offset, uint8_t data);
long download_to_flash(unsigned long offset);
// int check_firmware(unsigned long size);
#define GD32F4XX_FLASH_SIZE (1024*1024)

#endif
