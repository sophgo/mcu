#ifndef __EEPROM__
#define __EEPROM__

#include <stdint.h>
#include <libopencm3/stm32/flash.h>

#define EEPROM_BASE	0x08080000
#define EEPROM_SIZE	0x00000800

#define EEPROM_CELL_SIZE	32
#define EEPROM_CELL_OFFSET(n)	(EEPROM_CELL_SIZE * n)
#define EEPROM_CELL_ADDR(n)	(EEPROM_BASE + EEPROM_CELL_OFFSET(n))

#define EEPROM_MAC0_OFFSET		EEPROM_CELL_OFFSET(0)
#define EEPROM_MAC1_OFFSET		EEPROM_CELL_OFFSET(1)
#define EEPROM_SN_OFFSET		EEPROM_CELL_OFFSET(2)
#define EEPROM_BOARD_TYPE_OFFSET	EEPROM_CELL_OFFSET(3)
#define EEPROM_POWER_OFF_REASON_OFFSET	EEPROM_CELL_OFFSET(5)

#define EEPROM_MAC0_ADDR		EEPROM_CELL_ADDR(0)
#define EEPROM_MAC1_ADDR		EEPROM_CELL_ADDR(1)
#define EEPROM_SN_ADDR			EEPROM_CELL_ADDR(2)
#define EEPROM_BOARD_TYPE_ADDR		EEPROM_CELL_ADDR(3)
#define EEPROM_POWER_OFF_REASON_ADDR	EEPROM_CELL_ADDR(5)


static inline uint8_t eeprom_read_byte(uint32_t offset)
{
	return *((volatile uint8_t *)(EEPROM_BASE + offset));
}


static inline void eeprom_write_byte(uint32_t offset, uint8_t data)
{
	uint8_t flash_data[FLASH_PAGE_SIZE] = {0};
	volatile uint8_t *p_eeprom = (uint8_t *) EEPROM_BASE;

	int i = 0;

	for (i = 0; i < FLASH_PAGE_SIZE; i++){
		flash_data[i] = p_eeprom[i];
	}

	flash_data[offset] = data;
	flash_program_page(EEPROM_BASE, flash_data);
}

#endif
