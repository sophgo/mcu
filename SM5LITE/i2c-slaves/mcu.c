#include <libopencm3/cm3/nvic.h>
#include <i2c_master.h>
#include <string.h>
#include <project.h>
#include <i2c_slave.h>
#include <common.h>
#include <mon.h>
#include <upgrade.h>
#include <loop.h>
#include <chip.h>
#include <eeprom.h>
#include <power.h>
#include <wdt.h>

#define REG_BOARD_TYPE		0x00
#define REG_SW_VER		0x01
#define REG_HW_VER		0x02
#define REG_CMD			0x03
#define REG_INT_STATUS		0x06
#define REG_INT_MASK		0x08
#define REG_SOC_RST_TIMES	0x0a
#define REG_UPTIME_LO		0x0b
#define REG_UPTIME_HI		0x0c

#define REG_GP0			0x14
#define REG_DDR			0x15
#define REG_PWR_GOOD		0x16
#define REG_MODE_FLAG		0x17

#define REG_CURRENT_LO		0x28
#define REG_CURRENT_HI		0x29

#define REG_STAGE		0x3c
#define REG_EEPROM_OFFSET_LO	0x3e	/* 16bit eeprom address, low 8bits */
#define REG_EEPROM_OFFSET_HI	0x3f	/* 16bit eeprom address, high 8bits */
#define REG_EEPROM_DATA		0x40	/* eeprom data */
#define REG_EEPROM_LOCK		0x60	/* eeprom write lock */
#define MCU_REG_MAX		0x64

#define MCU_EEPROM_DATA_MAX	0x20

struct mcu_ctx {
	int set_idx;
	int idx;
	int cmd_tmp;
	uint8_t cmd;
	uint8_t int_status;
	uint8_t gp0;
	uint8_t eeprom_offset_l, eeprom_offset_h;

	uint16_t tmp;
};

static struct mcu_ctx mcu_ctx;

void mcu_raise_interrupt(uint8_t interrupts)
{
	mcu_ctx.int_status |= interrupts;
}

void mcu_clear_interrupt(uint8_t interrupts)
{
	mcu_ctx.int_status &= ~interrupts;
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

	i2c_peripheral_disable(I2C1);
	nvic_disable_irq(NVIC_I2C1_IRQ);
	switch (mcu_ctx.cmd) {
	case CMD_POWER_OFF:
		eeprom_log_power_off_reason(EEPROM_POWER_OFF_REASON_POWER_OFF);
		wdt_reset();
		root_power_off();
		break;
	case CMD_RESET:
		eeprom_log_power_off_reason(EEPROM_POWER_OFF_REASON_RESET);
		chip_reset();
		wdt_reset();
		break;
	case CMD_REBOOT:
		eeprom_log_power_off_reason(EEPROM_POWER_OFF_REASON_REBOOT);
		wdt_reset();
		root_power_reboot();
		break;
	case CMD_UPDATE:
		nvic_enable_irq(NVIC_I2C1_IRQ);
		i2c_upgrade_start();
		break;
	default:
		break;
	}
	mcu_ctx.cmd = 0;
	mcu_ctx.cmd_tmp = 0;
	i2c_peripheral_enable(I2C1);
	nvic_enable_irq(NVIC_I2C1_IRQ);
}

static inline uint16_t eeprom_offset(struct mcu_ctx *ctx)
{
	uint16_t offset_base = ctx->eeprom_offset_l |
		(ctx->eeprom_offset_h << 8);
	uint16_t offset_off =
		ctx->idx - REG_EEPROM_DATA;
	return offset_base + offset_off;
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
	default:
		break;
	}

	idx_inc(ctx);
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
		ret = WORK_MODE_SOC;
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

void mcu_init(struct i2c_slave_ctx *i2c_slave_ctx)
{
	loop_add(mcu_process);
	i2c_slave_register(i2c_slave_ctx, &slave);
}

