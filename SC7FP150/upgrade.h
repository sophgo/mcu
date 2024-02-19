#ifndef __UPGRADE_H__
#define __UPGRADE_H__
/* note: the size must be the same as layout.xml*/
#define MEMMAP_FLASH_START	(0x08000000)
#define MEMMAP_FLASH_SIZE	(80 * 1024)
#define MEMMAP_FLASH_END	(MEMMAP_FLASH_START + MEMMAP_FLASH_SIZE)

#define MEMMAP_LOADER_START	MEMMAP_FLASH_START
#define MEMMAP_LOADER_SIZE	(32 * 1024)
#define MEMMAP_LOADER_END	(MEMMAP_LOADER_START + MEMMAP_LOADER_SIZE)

#define MEMMAP_EFIT_START	MEMMAP_LOADER_END
#define MEMMAP_EFIT_SIZE	(8 * 1024)
#define MEMMAP_EFIT_END		(MEMMAP_EFIT_START + MEMMAP_EFIT_SIZE)

#define MEMMAP_APP_START	(MEMMAP_EFIT_END)
#define MEMMAP_APP_SIZE		(32 * 1024)
#define MEMMAP_APP_END		(MEMMAP_APP_START + MEMMAP_APP_SIZE)

#define MEMMAP_TAG_START	(MEMMAP_TAG_END - MEMMAP_TAG_SIZE)
#define MEMMAP_TAG_SIZE		(128)
#define MEMMAP_TAG_END		(MEMMAP_FLASH_END)

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
