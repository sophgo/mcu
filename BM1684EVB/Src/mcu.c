/*
 * mcu.c
 *
 *  Created on: Apr 25, 2019
 *      Author: weic
 */

#include <assert.h>
#include "main.h"
#include "i2c_bm.h"

#include "eeprom.h"
uint32_t addr_debug = 0x08080010;
extern void EEPROM_Write(uint32_t Addr, uint32_t writeFlashData);

struct mcu_map {
	uint8_t mcu_id;
	uint8_t mcu_ver;
	uint8_t pcb_ver;
	uint8_t ctrl;
	int8_t soc_temp; /* soc, temperature */
	int8_t board_temp;
	uint8_t alert_mask[2];
	uint8_t alert[2];
	uint8_t reset_count;
	uint8_t uptime[2];
	uint8_t reset_reason;
	uint8_t	rtc[6];
};

static struct mcu_ctx {
	int set_idx;
	int idx;
	struct mcu_map map;
} mcu_ctx = {
	.map = {
		.mcu_id = 0x01,
		.mcu_ver = 0x02,
		.pcb_ver = 0x03,
		.reset_reason = 0x0f,
		.soc_temp = 20,
		.board_temp = -20,
		.uptime = {0x01, 0x02},
		.alert_mask = {0x02, 0x03},
		.alert = {0x1c, 0x00},
		.reset_count = 100,
	},
};

#define MCU_REG_MAX 20
#define MCU_REG_CTL 3

static void mcu_match(int dir)
{
	if (dir == I2C_SLAVE_WRITE) {
		mcu_ctx.set_idx = 1;
	}
}

static inline void idx_set(uint8_t idx)
{
	mcu_ctx.idx = idx % MCU_REG_MAX;
}
static inline void idx_inc(void)
{
	mcu_ctx.idx = (mcu_ctx.idx + 1) % MCU_REG_MAX;
}

//CPLD Command
#define CMD_CPLD_PWR_ON			0x01       //1684 power on
#define CMD_CPLD_PWR_DOWN		0x02	   //1684 power down
#define CMD_CPLD_1684RST		0x03       //reset 1684
#define CMD_CPLD_SWRST			0x04	   //soft resetting
#define CMD_CPLD_CLR			0x05       // clean MCU_ERR_INT, set 0.
//BM1684 Command
#define CMD_BM1684_REBOOT		0x06       // power is always on
#define CMD_BM1684_RST			0x07       // power down

static void mcu_write(volatile uint8_t data)
{
	if (mcu_ctx.set_idx) {
		mcu_ctx.idx = (int)data % sizeof(struct mcu_map);
		mcu_ctx.set_idx = 0;
		return;
	}
	*((uint8_t *)(&mcu_ctx.map) + mcu_ctx.idx) = data;

	switch (data) {
		  case CMD_CPLD_PWR_ON:
			  PowerON();
			  break;
		  case CMD_CPLD_PWR_DOWN:
			  PowerDOWN();
			  break;
		  case CMD_CPLD_1684RST:
			  BM1684_RST();
//			  i2c_regs.rst_1684_times++;
			  break;
		  case CMD_CPLD_SWRST:
			  break;
		  case CMD_CPLD_CLR:
//			  Clean_ERR_INT();
			  break;
		  case CMD_BM1684_REBOOT:
			  break;
		  case CMD_BM1684_RST:
			  BM1684_RST();
			  break;
		}

	    if (mcu_ctx.idx != MCU_REG_CTL) {
			mcu_ctx.idx = (mcu_ctx.idx + 1) % sizeof(struct mcu_map);
		}

		idx_inc();
}

static uint8_t mcu_read(void)
{
	uint8_t tmp = *((uint8_t *)(&mcu_ctx.map) + mcu_ctx.idx);
	mcu_ctx.idx = (mcu_ctx.idx + 1) % sizeof(struct mcu_map);
	return tmp;
}

static void mcu_stop(void)
{
}

static struct i2c_slave_op slave = {
	.addr = 0x17,	/* mcu common slave address */
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
};

void mcu_init(void)
{
	assert(sizeof(struct mcu_map) == 0x14);
	i2c_slave_register(&slave);
}


