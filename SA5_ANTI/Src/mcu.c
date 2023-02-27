/*
 * mcu.c
 *
 *  Created on: Apr 25, 2019
 *      Author: weic
 */

#include <assert.h>
#include "main.h"
#include "i2c_slave.h"
#include "eeprom.h"
#include "debug.h"
#include "upgrade.h"
#include "string.h"
#include "soc_eeprom.h"
#include "mcu.h"

static unsigned short uptime;

struct mcu_ctx {
	int set_idx;
	int idx;
	int load_cmd_reg;
	I2C_REGS *map;
};

I2C_REGS mcu_reg;
static struct mcu_ctx mcu1_ctx;
static struct mcu_ctx mcu3_ctx;

#define MCU_REG_MAX REG_NUMBER

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
	if (dir == I2C_SLAVE_WRITE) {
		ctx->set_idx = 1;
	}
}

static inline uint16_t eeprom_offset(struct mcu_ctx *ctx)
{
	uint16_t offset_base = ctx->map->eeprom_offset_l |
		(ctx->map->eeprom_offset_h << 8);
	uint16_t offset_off =
		ctx->idx - REG_EEPROM_DATA;
	return offset_base + offset_off;
}
void mcu_tick_isr(void)
{
	uptime++;
	i2c_regs.uptime0 = uptime & 0xFF;
	i2c_regs.uptime1 = ( uptime >> 8 )& 0xFF;
}

static void mcu_process_cmd_slow_start(void)
{
	/* faster than execute switch-case every times*/
	if (i2c_regs.cmd_reg == 0)
		return;

	switch (i2c_regs.cmd_reg) {
		/* fast calls */
		case CMD_CPLD_SWRST:
			i2c_regs.cmd_reg = 0;
			break;
		case CMD_CPLD_CLR:
			i2c_regs.cmd_reg = 0;
			break;
		/* delayed calles */
		case CMD_CPLD_PWR_ON:
			if (i2c_regs.power_good) {
				i2c_regs.cmd_reg = 0;
				break;
			}
		case CMD_CPLD_PWR_DOWN:
		case CMD_CPLD_1684RST:
		case CMD_BM1684_REBOOT:
		case CMD_MCU_UPDATE:
			break;
		default:
			/* invalid command */
			i2c_regs.cmd_reg = 0;
			break;
	}
}

void mcu_process_cmd_slow(void)
{
	if (i2c_regs.cmd_reg == 0)
		return;

	__HAL_I2C_DISABLE(&hi2c3);
	HAL_NVIC_DisableIRQ(I2C3_IRQn);

	switch (i2c_regs.cmd_reg) {
	case CMD_CPLD_PWR_ON:
		PowerON();
		break;
	case CMD_CPLD_PWR_DOWN:
		eeprom_log_power_off_reason(EEPROM_POWER_OFF_REASON_POWER_OFF);
		PowerDOWN();
		break;
	case CMD_CPLD_1684RST:
		eeprom_log_power_off_reason(EEPROM_POWER_OFF_REASON_RESET);
		BM1684_RST();
		i2c_regs.rst_1684_times++;
		break;
	case CMD_BM1684_REBOOT:
		eeprom_log_power_off_reason(EEPROM_POWER_OFF_REASON_REBOOT);
		BM1684_REBOOT();
		break;
	case CMD_MCU_UPDATE:
		HAL_NVIC_EnableIRQ(I2C3_IRQn);
		upgrade_start();
		break;
	}

	i2c_regs.cmd_reg = 0;

	HAL_NVIC_EnableIRQ(I2C3_IRQn);
	__HAL_I2C_ENABLE(&hi2c3);
}

static void mcu_write(void *priv, volatile uint8_t data)
{
	struct mcu_ctx *ctx = priv;

	if (ctx->set_idx) {
		idx_set(ctx, data);
		ctx->set_idx = 0;
		return;
	}
	I2C_REGS *map = ctx->map;
	*(((uint8_t *)map) + ctx->idx) = data;

	switch (ctx->idx) {
	case REG_CMD_REG:
		ctx->load_cmd_reg = data;
		break;
	case REG_CMD:
		map->cmd = data;
		break;
	case REG_EEPROM_OFFSET_L:
		map->eeprom_offset_l = data;
		break;
	case REG_EEPROM_OFFSET_H:
		map->eeprom_offset_h = data;
		break;
	case REG_EEPROM_DATA ...
		(REG_EEPROM_DATA + MCU_EEPROM_DATA_MAX - 1):
		eeprom_write_byte_protected(eeprom_offset(ctx), data);
		break;
	case REG_EEPROM_LOCK:
		eeprom_lock_code(data);
		break;
	case REG_CRITICAL_ACTIONS:
		i2c_regs.critical_action = data;
		break;
	case REG_CTRITICAL_TEMP:
		i2c_regs.critical_temp = data;
		break;
	case REG_REPOWERON_TEMP:
		i2c_regs.repoweron_temp = data;
		break;
	case REG_PMIC_POWEROFF:
		i2c_regs.pmic_poweroff = data;
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
	I2C_REGS *map = ctx->map;

	switch (ctx->idx) {
	case REG_VENDER:
		ret = i2c_regs.vender;
		break;
	case REG_SW_VER:
		ret = i2c_regs.sw_ver;
		break;
	case REG_HW_VER:
		ret = i2c_regs.hw_ver;
		break;
	case REG_CMD_REG:
		ret = i2c_regs.cmd_reg;
		break;
	case REG_TEMP1684:
		ret = i2c_regs.temp1684;
		break;
	case REG_TEMP_BOARD:
		ret = i2c_regs.temp_board;
		break;
	case REG_INTR_STATUS1:
		ret = i2c_regs.intr_status1;
		break;
	case REG_TEST_INTR_EN:
		ret = i2c_regs.test_intr_en;
		break;
	case REG_INTR_MASK1:
		ret = i2c_regs.intr_mask1;
		break;
	case REG_1684_RST_TIMES:
		ret = i2c_regs.rst_1684_times;
		break;
	case REG_UPTIME0:
		ret = i2c_regs.uptime0;
		break;
	case REG_UPTIME1:
		ret = i2c_regs.uptime1;
		break;
	case REG_CAUSE_PWR_DOWN:
		ret = i2c_regs.cause_pwr_down;
		break;
	case REG_CMD:
		ret = map->cmd;
		break;
	case REG_DDR:
		ret = i2c_regs.ddr;
		break;
	case REG_PWR_GOOD:
		ret = i2c_regs.power_good;
		break;
	case REG_EEPROM_OFFSET_L:
		ret = map->eeprom_offset_l;
		break;
	case REG_EEPROM_OFFSET_H:
		ret = map->eeprom_offset_h;
		break;
	case REG_EEPROM_DATA ...
		(REG_EEPROM_DATA + MCU_EEPROM_DATA_MAX - 1):
		EEPROM_ReadBytes(eeprom_offset(ctx), &ret, 1);
		break;
	case REG_EEPROM_LOCK:
		ret = eeprom_get_lock_status();
		break;
	case REG_MODE_FLAG:
		ret = i2c_regs.mode_flag;
		break;
	case REG_STAGE:
		ret = i2c_regs.stage;
		break;
	case REG_CRITICAL_ACTIONS:
		ret = i2c_regs.critical_action;
		break;
	case REG_CTRITICAL_TEMP:
		ret = i2c_regs.critical_temp;
		break;
	case REG_REPOWERON_TEMP:
		ret = i2c_regs.repoweron_temp;
		break;
	case REG_PMIC_POWEROFF:
		ret = i2c_regs.pmic_poweroff;
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
	if (ctx->load_cmd_reg) {
		i2c_regs.cmd_reg = ctx->load_cmd_reg;
		ctx->load_cmd_reg = 0;
	}
	mcu_process_cmd_slow_start();
}

static void mcu_reset(void *priv)
{
	struct mcu_ctx *ctx = priv;

	memset(ctx, 0x00, sizeof(*ctx));
	ctx->map = &mcu_reg;
}

static struct i2c_slave_op slave = {
	.addr = 0x38,	/* mcu common slave address */
	.mask = 0x07,
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
	.reset = mcu_reset,
	.priv = &mcu1_ctx,
};

static struct i2c_slave_op slave3 = {
	.addr = 0x17,	/* mcu common slave address */
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
	.reset = mcu_reset,
	.priv = &mcu3_ctx,
};

void mcu_init(void)
{
	assert(sizeof(I2C_REGS) == 0x6c);
	mcu1_ctx.map = &mcu_reg;
	mcu3_ctx.map = &mcu_reg;
	uptime = 0;

	if (i2c_regs.vender == VENDER_SA5) {
		i2c_slave_register(&i2c_ctx1, &slave);
	}
	i2c_slave_register(&i2c_ctx3, &slave3);
}

void mcu_set_temp(int soc, int board)
{
	int8_t temp_soc, temp_board;

	if (soc < -64)
		temp_soc = -64;
	else if (soc > 127)
		temp_soc = 127;
	else
		temp_soc = soc;

	if (board < -64)
		temp_board = -64;
	else if (board > 127)
		temp_board = 127;
	else
		temp_board = board;

	i2c_regs.temp1684 = *(uint8_t *)&temp_soc;
	i2c_regs.temp_board = *(uint8_t *)&temp_board;
}

void set_mcu_default_feature(void)
{

	i2c_regs.repoweron_temp = 85;

	i2c_regs.critical_temp = 120;
	i2c_regs.critical_action = CRITICAL_ACTION_POWERDOWN;
}

int pmic_need_poweroff(void)
{
	return i2c_regs.pmic_poweroff;
}
