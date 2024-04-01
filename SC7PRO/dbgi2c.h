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

	struct {
		uint32_t voltage;
		uint32_t current;
		uint32_t power;
	} tpu;

	struct {
		uint32_t voltage;
		uint32_t current;
		uint32_t power;
	}vddc;

	struct {
		uint32_t voltage;
		uint32_t current;
		uint32_t power;
	}vdd_phy;

	uint32_t i12v_atx;
	uint32_t i12v_pcie;
	uint32_t i3v3_pcie;

	uint32_t rdrop_tpu; //60 0x3c
	uint32_t rdrop_vddc;//64 0x40
	uint32_t rdrop_vdd_phy;//68 044

} __attribute__((packed));


int dbgi2c_write32(int idx, uint64_t addr, uint32_t value);
int dbgi2c_read32(int idx, uint64_t addr, uint32_t *value);
int dbgi2c_read(int idx, uint64_t addr, void *data, int len);
void dbgi2c_test(void);
void dbgi2c_init(struct i2c_slave_ctx *i2c);
void dbgi2c_broadcast(int idx, struct dbgi2c_info *info);

#endif
