/*
 * mcu.c: CV84X6EVB I2C 从设备 — 板卡信息 + Flash 升级 + 温度
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_i2c.h>
#include <gd32e50x_fmc.h>
#include <i2c-slaves/mcu/mcu.h>
#include <i2c/i2c01_slave/i2c01_slave.h>
#include <slave/slave.h>
#include <common/common.h>
#include <project.h>
#include <upgrade/upgrade.h>
#include <loop/loop.h>
#include <debug.h>
#include <string.h>

/* 寄存器地址定义 */
#define REG_BOARD_TYPE		0x00
#define REG_SW_VER		0x01
#define REG_HW_VER		0x02
#define REG_DDR_TYPE		0x03
#define REG_BOM_VER		0x04
#define REG_CMD			0x05
#define REG_MCU_FAMILY		0x18
#define REG_TEMP_LOCAL		0x30
#define REG_TEMP_REMOTE		0x31
#define REG_FLASH_CMD		0x63
#define REG_FLASH_OFFSET	0x7c
#define REG_FLASH_DATA		0x80
#define REG_FLASH_FLUSH		0xff

#define FLASH_CMD_UNLOCK	0x02
#define FLASH_CMD_LOCK		0x03
#define FLASH_CMD_ERASE		0x04
#define MCU_REG_MAX		0x100

#define FLASH_SIZE		(256 * 1024)
#define FLASH_PAGE_SIZE		(8 * 1024)
#define FLASH_PAGE_MASK		(FLASH_PAGE_SIZE - 1)

extern int ct7451_remote_temp;
extern int ct7451_local_temp;

static struct mcu_ctx {
	int set_idx;
	int idx;
	uint8_t cmd;
	uint8_t flash_offset[4];
	uint8_t flash_data[128];
	int flash_flush;
} mcu_ctx;

static inline void idx_set(struct mcu_ctx *ctx, uint8_t idx)
{
	ctx->idx = idx % MCU_REG_MAX;
}

static inline void idx_inc(struct mcu_ctx *ctx)
{
	ctx->idx = (ctx->idx + 1) % MCU_REG_MAX;
}

static uint32_t flash_byte2u32(void *byte)
{
	uint8_t *p = byte;
	return p[3] | (p[2] << 8) | (p[1] << 16) | (p[0] << 24);
}

static void flash_flush_data(struct mcu_ctx *ctx)
{
	uint32_t addr = FLASH_BASE + (flash_byte2u32(ctx->flash_offset) & ~(128 - 1));
	uint32_t tmp;
	int i;

	for (i = 0; i < 128 / 4; ++i) {
		tmp = ((uint32_t *)ctx->flash_data)[i];
		if (tmp != 0xffffffff)
			fmc_word_program(addr + i * 4, tmp);
	}
}

static void flash_exec_cmd(struct mcu_ctx *ctx, int cmd)
{
	uint32_t offset;

	switch (cmd) {
	case FLASH_CMD_UNLOCK:
		fmc_unlock();
		break;
	case FLASH_CMD_LOCK:
		fmc_lock();
		break;
	case FLASH_CMD_ERASE:
		offset = flash_byte2u32(ctx->flash_offset);
		if (offset & FLASH_PAGE_MASK)
			break;
		if (offset >= FLASH_SIZE)
			break;
		fmc_page_erase(FLASH_BASE + offset);
		break;
	}
	fmc_flag_clear(FMC_FLAG_END);
}

static uint8_t flash_read_byte(struct mcu_ctx *ctx)
{
	uint32_t off = flash_byte2u32(ctx->flash_offset) + (ctx->idx - REG_FLASH_DATA);
	return *(uint8_t *)(FLASH_BASE + off);
}

/* --- i2c01_slave 回调 --- */

static void mcu_match(void *priv, int dir)
{
	struct mcu_ctx *ctx = priv;
	if (dir == I2C_SLAVE_WRITE)
		ctx->set_idx = 1;
}

static void mcu_write(void *priv, volatile uint8_t data)
{
	struct mcu_ctx *ctx = priv;

	if (ctx->set_idx) {
		idx_set(ctx, data);
		ctx->set_idx = 0;
		return;
	}

	switch (ctx->idx) {
	case REG_CMD:
		ctx->cmd = data;
		break;
	case REG_FLASH_CMD:
		flash_exec_cmd(ctx, data);
		break;
	case REG_FLASH_OFFSET + 0:
	case REG_FLASH_OFFSET + 1:
	case REG_FLASH_OFFSET + 2:
	case REG_FLASH_OFFSET + 3:
		ctx->flash_offset[ctx->idx - REG_FLASH_OFFSET] = data;
		break;
	case REG_FLASH_DATA ... REG_FLASH_FLUSH:
		ctx->flash_data[ctx->idx - REG_FLASH_DATA] = data;
		if (ctx->idx == REG_FLASH_FLUSH)
			ctx->flash_flush = 1;
		break;
	default:
		break;
	}
	idx_inc(ctx);
}

static uint8_t mcu_read(void *priv)
{
	struct mcu_ctx *ctx = priv;
	uint8_t data = 0;

	switch (ctx->idx) {
	case REG_BOARD_TYPE:
		data = get_board_type();
		break;
	case REG_SW_VER:
		data = get_firmware_version();
		break;
	case REG_HW_VER:
		data = get_pcb_version();
		break;
	case REG_DDR_TYPE:
		data = get_ddr_type();
		break;
	case REG_BOM_VER:
		data = get_bom_version();
		break;
	case REG_CMD:
		data = 0;
		break;
	case REG_MCU_FAMILY:
		data = MCU_FAMILY_GD32E50;
		break;
	case REG_TEMP_LOCAL:
		data = (uint8_t)ct7451_local_temp;
		break;
	case REG_TEMP_REMOTE:
		data = (uint8_t)ct7451_remote_temp;
		break;
	case REG_FLASH_OFFSET + 0:
	case REG_FLASH_OFFSET + 1:
	case REG_FLASH_OFFSET + 2:
	case REG_FLASH_OFFSET + 3:
		data = ctx->flash_offset[ctx->idx - REG_FLASH_OFFSET];
		break;
	case REG_FLASH_DATA ... REG_FLASH_FLUSH:
		data = flash_read_byte(ctx);
		break;
	default:
		data = 0xff;
		break;
	}
	idx_inc(ctx);
	return data;
}

static void mcu_stop(void *priv)
{
	struct mcu_ctx *ctx = priv;
	if (ctx->flash_flush) {
		flash_flush_data(ctx);
		ctx->flash_flush = 0;
	}
}

static void mcu_reset(void *priv)
{
	memset(priv, 0, sizeof(struct mcu_ctx));
}

static struct i2c01_slave_op slave = {
	.addr   = MCU_SLAVE_ADDR,
	.mask   = 0x00,
	.match  = mcu_match,
	.write  = mcu_write,
	.read   = mcu_read,
	.stop   = mcu_stop,
	.reset  = mcu_reset,
	.priv   = &mcu_ctx,
};

/* --- 公开接口 --- */

void mcu_init(struct i2c01_slave_ctx *i2c_slave_ctx)
{
	memset(&mcu_ctx, 0, sizeof(mcu_ctx));
	slave.addr = MCU_SLAVE_ADDR;
	i2c01_slave_register(i2c_slave_ctx, &slave);
	loop_add(mcu_process);
}

void mcu_process(void)
{
	/* mcu 处理函数，由 loop 轮询调用 */
	/* 温度数据在 ct7451_process() 中更新 */
}
