/*
 * mcu.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <gd32e50x_i2c.h>
#include <i2c-slaves/mcu/mcu.h>
#include <stdio.h>
#include <i2c/i2c01_slave/i2c01_slave.h>
#include <slave/slave.h>
#include <string.h>
#include <tick/tick.h>
#include <timer/timer.h>
#include <debug.h>
#include <chip/chip.h>
#include <common/common.h>
#include <power/power.h>
#include <project.h>
#include <upgrade/upgrade.h>
#include <loop/loop.h>
#include <gpio/gpio.h>

#define REG_BOARD_TYPE			0x00
#define REG_SW_VER			0x01
#define REG_HW_VER			0x02
#define REG_CMD				0x03
#define REG_INT_STATUS1			0x06
#define REG_INT_STATUS2			0x07
#define REG_INT_MASK1			0x08
#define REG_INT_MASK2 			0x09

#define REG_SOC_RST_TIMES		0x0a
#define REG_UPTIME_LO			0x0b
#define REG_UPTIME_HI			0x0c
#define REG_POWER_OFF_REASON		0x0d

#define RTC_SECONDS			0x0e
#define RTC_MINUTES			0x0f
#define RTC_HOURS			0x10
#define RTC_DATE			0x11
#define RTC_MONTH			0x12
#define RTC_YEAR			0x13

#define REG_MCU_FAMILY			0x18

#define A2_TMP_OVER_REPORT		1<<0
#define POWER_68127_TMP_OVER_REPORT	1<<1
#define BOARD_TMP_OVER_REPORT		1<<2
#define BOARD_TMP_OVER_POWER_OFF	1<<3
#define A2_TMP_OVER_POWER_OFF		1<<4
#define SYS_POWER_EXCEPTION_POWER_OFF	1<<5
#define V12_EXCEPTION_POWER_OFF		1<<6
#define A2_REBOOT_CMD			1<<7

#define WATCH_DOG 			1<<0
#define TEST_INT			1<<1

#define I_12V_ATX_L			0x28
#define REG_CRITICAL_ACTIONS		0x65
#define REG_CRITICAL_TEMP		0x66
#define REG_REPOWER_ON_TEMP		0x67

#define REG_FLASH_CMD			0x63
#define REG_FLASH_OFFSET		0x7c
#define REG_FLASH_DATA			0x80
#define REG_FLASH_FLUSH			0xff

#define FLASH_CMD_UNLOCK		0x02
#define FLASH_CMD_LOCK			0x03
#define FLASH_CMD_ERASE			0x04

#define MCU_REG_MAX			0x100
#define CMD_POWER_OFF			0x02
#define CMD_RESET			0x03	// drag reset pin
#define CMD_REBOOT			0x07	// power off - power on
#define CMD_UPDATE			0x08

extern int power_is_on;

int is_print_enabled;
static struct mcu_ctx mcu_ctx;

struct mcu_ctx {
	int set_idx;
	int idx;
	int cmd_tmp;
	uint8_t cmd;
	uint16_t tmp;
	uint8_t int_status[2];
	uint8_t int_mask[2];
	uint8_t critical_action;
	uint8_t	repower_on_temp;
	uint8_t critical_temp;
	uint8_t power_off_reason;
	uint8_t __attribute__((aligned(4))) flash_offset[4];
	uint8_t __attribute__((aligned(4))) flash_data[128];
	int flash_flush;
};

static struct mcu_ctx mcu_ctx;

static inline void idx_set(struct mcu_ctx *ctx, uint8_t idx)
{
	ctx->idx = idx % MCU_REG_MAX;
}

static inline void idx_inc(struct mcu_ctx *ctx)
{
	ctx->idx = (ctx->idx + 1) % MCU_REG_MAX;
}

static inline uint32_t flash_byte2u32(void *byte)
{
	uint8_t *p;

	p = byte;
	/* big endian */
	return p[3] | (p[2] << 8) | (p[1] << 16) | (p[0] << 24);
}

static inline uint32_t flash_offset(struct mcu_ctx *ctx)
{
	return flash_byte2u32(ctx->flash_offset) + (ctx->idx - REG_FLASH_DATA);
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
		/* not page aligned */
		if (offset & FLASH_PAGE_MASK)
			break;

		/* overflow */
		if (offset >= FLASH_SIZE)
			break;

		fmc_page_erase(FLASH_BASE + offset);
		break;
	}
	fmc_flag_clear(FMC_FLAG_END);
}

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
		ctx->cmd_tmp = data;
		break;
	case REG_CRITICAL_ACTIONS:
		ctx->critical_action = data;
		break;
	case REG_REPOWER_ON_TEMP:
		ctx->repower_on_temp = data;
		break;
	case REG_CRITICAL_TEMP:
		ctx->critical_temp = data;
		break;
	case REG_FLASH_CMD:
		flash_exec_cmd(ctx, data);
		break;
	case REG_FLASH_OFFSET + 0:
		ctx->flash_offset[0] = data;
		break;
	case REG_FLASH_OFFSET + 1:
		ctx->flash_offset[1] = data;
		break;
	case REG_FLASH_OFFSET + 2:
		ctx->flash_offset[2] = data;
		break;
	case REG_FLASH_OFFSET + 3:
		ctx->flash_offset[3] = data;
		break;
	case REG_FLASH_DATA ... REG_FLASH_FLUSH:
		ctx->flash_data[ctx->idx - REG_FLASH_DATA] = data;
		if (ctx->idx == REG_FLASH_FLUSH)
			ctx->flash_flush = true;
		break;
	default:
		break;
	}

	idx_inc(ctx);
}

static inline uint8_t flash_read_byte(struct mcu_ctx *ctx)
{
	return *(uint8_t *)(FLASH_BASE + flash_offset(ctx));
}

static uint8_t mcu_read(void *priv)
{
	struct mcu_ctx *ctx = priv;
	uint8_t ret = 0;

	switch (ctx->idx) {
	case REG_BOARD_TYPE:
		ret = get_board_type();
		break;
	case REG_SW_VER:
		ret = get_firmware_version();
		break;
	case REG_CMD:
		ret = 0;
		break;
	case REG_INT_STATUS1:
		ret = ctx->int_status[0];
		break;
	case REG_INT_STATUS2:
		ret = ctx->int_status[1];
		break;
	case REG_INT_MASK1:
		ret = ctx->int_mask[0];
		break;
	case REG_INT_MASK2:
		ret = ctx->int_mask[1];
		break;
	case REG_SOC_RST_TIMES:
		ret = chip_reset_times() & 0xff;
		break;
	case REG_UPTIME_LO:
		ctx->tmp = chip_uptime();
		ret = ctx->tmp & 0xff;
		break;
	case REG_UPTIME_HI:
		ret = (ctx->tmp >> 8) & 0xff;
		break;
	case REG_POWER_OFF_REASON:
		ret = ctx->power_off_reason;
	case REG_MCU_FAMILY:
		ret = MCU_FAMILY_GD32E50;
		break;
	case REG_CRITICAL_ACTIONS:
		ret = ctx->critical_action;
		break;
	case REG_CRITICAL_TEMP:
		ret = ctx->critical_temp;
		break;
	case REG_REPOWER_ON_TEMP:
		ret = ctx->repower_on_temp;
		break;
	case REG_FLASH_OFFSET + 0:
		ret = ctx->flash_offset[0];
		break;
	case REG_FLASH_OFFSET + 1:
		ret = ctx->flash_offset[1];
		break;
	case REG_FLASH_OFFSET + 2:
		ret = ctx->flash_offset[2];
		break;
	case REG_FLASH_OFFSET + 3:
		ret = ctx->flash_offset[3];
		break;
	case REG_FLASH_DATA ... REG_FLASH_FLUSH:
		ret = flash_read_byte(ctx);
		break;
	default:
		ret = 0xff;
		break;
	}

	idx_inc(ctx);
	return ret;
}

static void mcu_stop(void *priv)
{
	struct mcu_ctx *ctx = priv;
	ctx->cmd = ctx->cmd_tmp;

	if (ctx->flash_flush) {
		flash_flush_data(ctx);
		ctx->flash_flush = false;
	}
}

static void mcu_reset(void *priv)
{
	struct mcu_ctx *ctx = priv;
	memset(ctx, 0x00, sizeof(*ctx));
}

static struct i2c01_slave_op slave = {
	.addr = 0x17,	/* mcu common slave address */
	.mask = 0x00,
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
	.reset = mcu_reset,
	.priv = &mcu_ctx,
};

void mcu_a2evb_init(struct i2c01_slave_ctx *i2c_slave_ctx)
{
	mcu_ctx.critical_action = CRITICAL_ACTION_REBOOT;
	mcu_ctx.critical_temp = 80;
	mcu_ctx.repower_on_temp = 55;
	mcu_ctx.power_off_reason = 0;
	slave.addr = 0x17;
	i2c01_slave_register(i2c_slave_ctx, &slave);

	loop_add(mcu_process);
}

void mcu_process(void)
{
	/* Main loop query */
	if (mcu_ctx.cmd == 0)
		return;

	i2c_disable(I2C0);
	nvic_disable_irq(I2C0_EV_IRQn);
	switch (mcu_ctx.cmd) {
	case CMD_POWER_OFF:
		power_off();
		//if (is_evb_pwr_wakeup_H_on() == true)
		power_is_on = true;
		timer_delay_ms(500);
		mcu_ctx.power_off_reason = POWER_OFF_REASON_POWER_OFF;
		break;
	case CMD_RESET:
		chip_reset();
		mcu_ctx.power_off_reason = POWER_OFF_REASON_RESET;
		break;
	case CMD_REBOOT:
		chip_popd_reset_early();
		//if (is_evb_pwr_wakeup_H_on() == true)
		power_is_on = true;
		set_need_power_on();
		mcu_ctx.power_off_reason = POWER_OFF_REASON_REBOOT;
		break;
	default:
		break;
	}
	mcu_ctx.cmd = 0;
	mcu_ctx.cmd_tmp = 0;
	i2c_enable(I2C0);
	slave_init();
}

uint8_t get_critical_action(void)
{
	return mcu_ctx.critical_action;
}

uint8_t get_critical_temp(void)
{
	return mcu_ctx.critical_temp;
}

uint8_t get_repower_on_temp(void)
{
	return mcu_ctx.repower_on_temp;
}
