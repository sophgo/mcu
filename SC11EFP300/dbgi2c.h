#ifndef __DBGI2C_H__
#define __DBGI2C_H__

#include <i2c_slave.h>

struct dbgi2c_info {
	uint32_t tag;
	uint8_t id;
	uint8_t type;
	uint8_t sw_ver;
	uint8_t hw_ver;
	uint32_t i12v;

	uint32_t i12v_atx;
	uint32_t i12v_pcie;
	uint32_t i3v3_pcie;

	int soc_temp;
	int board_temp;

} __attribute__((packed));


int dbgi2c_write32(int idx, uint64_t addr, uint32_t value);
int dbgi2c_read32(int idx, uint64_t addr, uint32_t *value);
int dbgi2c_read(int idx, uint64_t addr, void *data, int len);
// void dbgi2c_test(void);
void dbgi2c_init(struct i2c_slave_ctx *i2c);
void dbgi2c_broadcast(int idx, struct dbgi2c_info *info);

#endif
