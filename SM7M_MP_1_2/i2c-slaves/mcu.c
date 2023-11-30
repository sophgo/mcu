#include <i2c_master.h>
#include <string.h>
#include <project.h>
#include <i2c_slave.h>
#include <i2c01_slave.h>
#include <common.h>
#include <mon.h>
#include <upgrade.h>
#include <loop.h>
#include <chip.h>
#include <eeprom.h>
#include <power.h>
#include <wdt.h>
#include <common.h>
#include <mcu.h>
#include <stdio.h>

#include <stdbool.h>

#define REG_BOARD_TYPE		0x00
#define REG_SW_VER		0x01
#define REG_HW_VER		0x02
#define REG_CMD			0x03
#define REG_SOC_TEMP		0x04
#define REG_BOARD_TEMP		0x05
#define REG_INT_STATUS		0x06
#define REG_TEST_INTR_EN	0x07
#define REG_INT_MASK		0x08
#define REG_SOC_RST_TIMES	0x0a
#define REG_UPTIME_LO		0x0b
#define REG_UPTIME_HI		0x0c

#define REG_GP0			0x14
#define REG_DDR			0x15
#define REG_PWR_GOOD		0x16
#define REG_MODE_FLAG		0x17
#define REG_MCU_FAMILY		0x18

#define REG_POWER_LO		0x24
#define REG_POWER_HI		0x25
#define REG_CURRENT_LO		0x28
#define REG_CURRENT_HI		0x29

#define REG_STAGE		0x3c
#define REG_EEPROM_OFFSET_LO	0x3e	/* 16bit eeprom address, low 8bits */
#define REG_EEPROM_OFFSET_HI	0x3f	/* 16bit eeprom address, high 8bits */
#define REG_EEPROM_DATA		0x40	/* eeprom data */
#define REG_EEPROM_LOCK		0x60	/* eeprom write lock */

#define MCU_REG_MAX		0x100

#define MCU_EEPROM_DATA_MAX	0x20

#define REG_FLASH_CMD		0x63
#define REG_FLASH_OFFSET	0x7c
#define REG_FLASH_DATA		0x80
#define REG_FLASH_FLUSH		0xff

#define FLASH_CMD_UNLOCK	0x02
#define FLASH_CMD_LOCK		0x03
#define FLASH_CMD_ERASE		0x04

struct mcu_ctx {
	int set_idx;
	int idx;
	int cmd_tmp;
	uint8_t cmd;
	uint8_t int_status;
	uint8_t gp0;
	uint8_t eeprom_offset_l, eeprom_offset_h;

	uint8_t __attribute__((aligned(4))) flash_offset[4];
	uint8_t __attribute__((aligned(4))) flash_data[128];
	int flash_flush;

	uint16_t tmp;
};

static struct mcu_ctx mcu_ctx;

void mcu_raise_interrupt(uint8_t interrupts)
{
	mcu_ctx.int_status |= interrupts;
	if (mcu_ctx.int_status)
		gpio_set(MCU_INT_PORT, MCU_INT_PIN);
}

void mcu_clear_interrupt(uint8_t interrupts)
{
	mcu_ctx.int_status &= ~interrupts;
	if (mcu_ctx.int_status == 0)
		gpio_clear(MCU_INT_PORT, MCU_INT_PIN);
}

static inline void idx_set(struct mcu_ctx *ctx, uint8_t idx)
{
	ctx->idx = idx % MCU_REG_MAX;
}

static inline void idx_inc(struct mcu_ctx *ctx)
{
	ctx->idx = (ctx->idx + 1) % MCU_REG_MAX;
}

static void mcu_match(void *priv, int dir)
{
	struct mcu_ctx *ctx = priv;

	if (dir == I2C_SLAVE_WRITE)
		ctx->set_idx = 1;
}

#define CMD_POWER_OFF		0x02
#define CMD_RESET		0x03	// drag reset pin
#define CMD_REBOOT		0x07	// power off - power on
#define CMD_UPDATE		0x08

void mcu_process(void)
{
	if (mcu_ctx.cmd == 0)
		return;
	i2c_disable(I2C2);
	nvic_irq_disable(MCU_SOC_I2C_IRQ);
	switch (mcu_ctx.cmd) {
	case CMD_POWER_OFF:
		power_off();
		wdt_reset();
		break;
	case CMD_RESET:
		eeprom_log_power_off_reason(EEPROM_POWER_OFF_REASON_RESET);
		chip_reset();
		wdt_reset();
		break;
	case CMD_REBOOT:
		eeprom_log_power_off_reason(EEPROM_POWER_OFF_REASON_REBOOT);
		chip_popd_reset();
		wdt_reset();
		break;
	default:
		break;
	}
	mcu_ctx.cmd = 0;
	mcu_ctx.cmd_tmp = 0;
	i2c_enable(I2C2);
	nvic_irq_enable(MCU_SOC_I2C_IRQ, 0, 0);
}

static inline uint16_t eeprom_offset(struct mcu_ctx *ctx)
{
	uint16_t offset_base = ctx->eeprom_offset_l |
		(ctx->eeprom_offset_h << 8);
	uint16_t offset_off =
		ctx->idx - REG_EEPROM_DATA;
	return offset_base + offset_off;
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
	case REG_INT_STATUS:
		mcu_clear_interrupt(data);
		break;
	case REG_TEST_INTR_EN:
		if (data == 0x5a)
			mcu_raise_interrupt(MCU_INT_TEST_INTR);
		break;
	case REG_GP0:
		ctx->gp0 = data;
		break;
	case REG_EEPROM_OFFSET_LO:
		ctx->eeprom_offset_l = data;
		break;
	case REG_EEPROM_OFFSET_HI:
		ctx->eeprom_offset_h = data;
		break;
	case REG_EEPROM_DATA ...
		(REG_EEPROM_DATA + MCU_EEPROM_DATA_MAX - 1):
		eeprom_write_byte_protected(eeprom_offset(ctx), data);
		break;
	case REG_EEPROM_LOCK:
		eeprom_lock_code(data);
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
	case REG_HW_VER:
		ret = get_hardware_version();
		break;
	case REG_CMD:
		ret = 0;
		break;
	case REG_SOC_TEMP:
		ret = get_soc_temp();
		break;
	case REG_BOARD_TEMP:
		ret = get_board_temp();
		break;
	case REG_INT_STATUS:
		ret = ctx->int_status;
		break;
	case REG_INT_MASK:
		/* mask is not supported */
		ret = 0;
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
	case REG_GP0:
		ret = ctx->gp0;
		break;
	case REG_DDR:
		ret = get_ddr_type();
		break;
	case REG_PWR_GOOD:
		ret = 1;
		break;
	case REG_MODE_FLAG:
		ret = get_work_mode();
		break;
	case REG_MCU_FAMILY:
		ret = MCU_FAMILY_GD32E50;
		break;
	case REG_POWER_LO:
		ctx->tmp = get_current() * 12;
		ret = ctx->tmp & 0xff;
		break;
	case REG_POWER_HI:
		ret = (ctx->tmp >> 8) & 0xff;
		break;
	case REG_CURRENT_LO:
		ctx->tmp = get_current();
		ret = ctx->tmp & 0xff;
		break;
	case REG_CURRENT_HI:
		ret = (ctx->tmp >> 8) & 0xff;
		break;
	case REG_STAGE:
		ret = get_stage();
		break;
	case REG_EEPROM_OFFSET_LO:
		ret = ctx->eeprom_offset_l;
		break;
	case REG_EEPROM_OFFSET_HI:
		ret = ctx->eeprom_offset_h;
		break;
	case REG_EEPROM_DATA ...
		(REG_EEPROM_DATA + MCU_EEPROM_DATA_MAX - 1):
		ret = eeprom_read_byte(eeprom_offset(ctx));
		break;
	case REG_EEPROM_LOCK:
		ret = eeprom_get_lock_status();
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

static struct i2c_slave_op slave = {
	.addr = 0x17,	/* mcu common slave address */
	.mask = 0x00,
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
	.reset = mcu_reset,
	.priv = &mcu_ctx,
};

static struct i2c01_slave_op slave01 = {
	.addr = 0x38,	/* mcu common slave address */
	.mask = 0x00,
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
	.reset = mcu_reset,
	.priv = &mcu_ctx,
};

void mcu_init(struct i2c_slave_ctx *i2c_slave_ctx,
	      struct i2c01_slave_ctx *i2c01_slave_ctx)
{
	loop_add(mcu_process);
	i2c_slave_register(i2c_slave_ctx, &slave);
	i2c01_slave_register(i2c01_slave_ctx, &slave01);
}

