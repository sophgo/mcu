#ifndef __EFUSE_H__
#define __EFUSE_H__
#include <stdint.h>

uint32_t efuse_read32(int chip, int efuse_id);
int efuse_write32(int chip, int efuse_id, uint32_t value);


#endif