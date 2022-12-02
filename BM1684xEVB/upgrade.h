#ifndef __UPGRADE_H__
#define __UPGRADE_H__

#include <libopencm3/stm32/flash.h>

#define MEMMAP_FLASH_START	FLASH_MEM_ADDR
#define MEMMAP_FLASH_SIZE	(64 * 1024)
#define MEMMAP_FLASH_END	(MEMMAP_FLASH_START + MEMMAP_FLASH_SIZE)

#define MEMMAP_LOADER_START	MEMMAP_FLASH_START
#define MEMMAP_LOADER_SIZE	(28 * 1024)
#define MEMMAP_LOADER_END	(MEMMAP_LOADER_START + MEMMAP_LOADER_SIZE)

#define MEMMAP_EFIT_START	MEMMAP_LOADER_END
#define MEMMAP_EFIT_SIZE	(4 * 1024)
#define MEMMAP_EFIT_END		(MEMMAP_EFIT_START + MEMMAP_EFIT_SIZE)

#define RUN_STAGE_APP		0
#define RUN_STAGE_LOADER	1
#define RUN_STAGE_UPGRADER	2
#define RUN_STAGE_UART_UPGRADER	3

int check_app(void);
int get_stage(void);
void i2c_upgrade_start(void);
void uart_upgrade_start(void);
void app_start(void);

#endif
