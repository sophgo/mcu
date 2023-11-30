#include <gd32e50x_fmc.h>
#include <flash.h>
#include <stdint.h>

/* data and len must 4 bytes align */
void flash_program_page(unsigned long addr, void *data, unsigned int len)
{
	int i;
	uint32_t tmp;

	fmc_unlock();

	fmc_page_erase(addr);

	for (i = 0; i < len / 4; ++i) {
		tmp = ((uint32_t *)data)[i];

		if (tmp != 0xffffffff)
			fmc_word_program(addr + i * 4, tmp);
	}

	fmc_lock();

	fmc_flag_clear(FMC_FLAG_END);
}

void flash_clear_error(void)
{
	fmc_flag_clear(FMC_FLAG_PGERR | FMC_FLAG_PGAERR |
		       FMC_WPERR | FMC_FLAG_END);
}


