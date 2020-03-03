/*
 * This file is part of the openfat project.
 *
 * Copyright (C) 2011  Department of Physics, University of Otago
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* MMC Card interface.
 */

#ifndef __MMC_H
#define __MMC_H

#include <stdint.h>

struct mmc_port {
	uint32_t spi;
	uint32_t cs_port;
	uint16_t cs_pin;
};

int mmc_init(uint32_t spi, uint32_t cs_port,
	     uint16_t cs_pin, struct mmc_port *mmc);

int mmc_read_sectors(const struct mmc_port *mmc,
		     uint32_t sector, uint32_t count, void *buf);
int mmc_write_sectors(const struct mmc_port *mmc,
		      uint32_t sector, uint32_t count, const void *buf);

uint16_t mmc_get_sector_size(const struct mmc_port *mmc);

#endif

