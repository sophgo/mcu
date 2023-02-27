#ifndef __FLASH_H__
#define __FLASH_H__

#include <stm32l0xx_hal.h>

#define FLASH_PROGRAM_START		(FLASH_BASE)
#define FLASH_PROGRAM_SIZE		(64 * 1024)
#define FLASH_PROGRAM_END		(FLASH_PROGRAM_START + FLASH_PROGRAM_SIZE)

#define FLASH_PAGE_MASK			(FLASH_PAGE_SIZE - 1)

/* must page align, page is 128bytes long */
void flash_program(unsigned long offset, void *data, int len);
void flash_init(void);

#endif /* FLASH_IF_H_ */
