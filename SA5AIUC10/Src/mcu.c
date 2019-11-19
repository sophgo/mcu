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
#include "rtc.h"
#include "debug.h"
#include "upgrade.h"
//uint32_t addr_debug = 0x08080010;
//extern void EEPROM_Write(uint32_t Addr, uint32_t writeFlashData);

static unsigned short uptime;

struct mcu_ctx {
	int set_idx;
	int idx;
	int setrtc;
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
	uint8_t tmp;

	/* faster than execute switch-case every times*/
	if (i2c_regs.cmd_reg == 0)
		return;
	if (CPLD_CLR_ERR & i2c_regs.cmd_reg)
		HAL_GPIO_WritePin(MCU_CPLD_ERR_GPIO_Port, MCU_CPLD_ERR_Pin, GPIO_PIN_RESET);
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
			HAL_NVIC_DisableIRQ(I2C1_IRQn);
			//HAL_NVIC_DisableIRQ(I2C3_IRQn);
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

	// now we have disabled i2c1 interrupt
	// put i2c1 controller to reset state
	__HAL_I2C_DISABLE(&hi2c1);
	HAL_Delay(1);	//three apb bus cycle is needed
	HAL_I2C_MspDeInit(&hi2c1);
	//HAL_I2C_MspDeInit(&hi2c3);
	switch (i2c_regs.cmd_reg) {
	case CMD_CPLD_PWR_ON:
		PowerON();
		break;
	case CMD_CPLD_PWR_DOWN:
		PowerDOWN();
		break;
	case CMD_CPLD_1684RST:
		BM1684_RST();
		i2c_regs.rst_1684_times++;
		break;
	case CMD_BM1684_REBOOT:
		BM1684_REBOOT();
		break;
	case CMD_MCU_UPDATE:
		HAL_I2C_MspInit(&hi2c1);
		upgrade_start();
		break;
	}
	i2c_regs.cmd_reg = 0;
	HAL_I2C_MspInit(&hi2c1);
	// re-enable i2c controller
	__HAL_I2C_ENABLE(&hi2c1);
	//HAL_I2C_MspInit(&hi2c3);
}

static void mcu_write(void *priv, volatile uint8_t data)
{
	struct mcu_ctx *ctx = priv;
	int offset;

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
		i2c_regs.cmd_reg_bkup = data;
		break;
	case REG_INTR_STATUS2:
		i2c_regs.intr_status2 = data;
		if (CPLD_SET_ERR | i2c_regs.intr_status2) {
			HAL_GPIO_WritePin(MCU_CPLD_ERR_GPIO_Port, MCU_CPLD_ERR_Pin, GPIO_PIN_SET);
		}
		break;
	case REG_SYS_RTC_YEAR ... (REG_SYS_RTC_YEAR + 5):
		offset = ctx->idx - REG_SYS_RTC_YEAR;
		map->rtc[offset] = data;
		ctx->setrtc = 1;
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
		EEPROM_WriteBytes(eeprom_offset(ctx), (uint8_t *)&data, 1);
		break;
	case REG_VENDER_VAL:
		i2c_regs.vender_val = data;
//		EEPROM_WriteBytes(VENDER_Addr, (uint8_t *)&data, 1);
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
	static RTC_DateTypeDef sDate;
	static RTC_TimeTypeDef sTime;
	int offset;
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
	case REG_INTR_STATUS2:
		ret = i2c_regs.intr_status2;
		break;
	case REG_INTR_MASK1:
		ret = i2c_regs.intr_mask1;
		break;
	case REG_INTR_MASK2:
		ret = i2c_regs.intr_mask2;
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
	case REG_SYS_RTC_YEAR ... (REG_SYS_RTC_YEAR + 5):
		offset = ctx->idx - REG_SYS_RTC_YEAR;
		if (offset < 3)
		{
			HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
			map->rtc[0] = sDate.Year;
			map->rtc[1] = sDate.Month;
			map->rtc[2] = sDate.Date;
		}
		else
		{
			HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
			map->rtc[3] = sTime.Hours;
			map->rtc[4] = sTime.Minutes;
			map->rtc[5] = sTime.Seconds;
		}
		ret = map->rtc[offset];
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
	case REG_VENDER_VAL:
		ret = i2c_regs.vender_val;
		break;
	case REG_PMIC_STATUS:
		ret = i2c_regs.pmic_status;
		break;
	case REG_PWRON_CMD:
		ret = i2c_regs.power_on_cmd;
		break;
	case REG_CMD_BKUP:
		ret = i2c_regs.cmd_reg_bkup;
		break;
	case REG_STAGE:
		ret = i2c_regs.stage;
		break;
	case REG_ERROR_LINE_L:
		ret = i2c_regs.error_line_l;
		break;
	case REG_ERROR_LINE_H:
		ret = i2c_regs.error_line_h;
		break;
	case REG_ERROR_CODE:
		ret = i2c_regs.error_code;
		break;
	case REG_I2C2_STATE:
		ret = hi2c2.State;
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
	if (ctx->setrtc)
	{
		ctx->setrtc = 0;
		RTC_DateTypeDef sDate;
		RTC_TimeTypeDef sTime;
		sDate.Year		= ctx->map->rtc[0];
		sDate.Month		= ctx->map->rtc[1];
		sDate.Date		= ctx->map->rtc[2];
		sTime.Hours		= ctx->map->rtc[3];
		sTime.Minutes	= ctx->map->rtc[4];
		sTime.Seconds	= ctx->map->rtc[5];

		HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
		HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	}
	if (ctx->load_cmd_reg) {
		i2c_regs.cmd_reg = ctx->load_cmd_reg;
		ctx->load_cmd_reg = 0;
	}
	mcu_process_cmd_slow_start();
}

static struct i2c_slave_op slave = {
	.addr = 0x38,	/* mcu common slave address */
	.mask = 0x07,
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
	.priv = &mcu1_ctx,
};

static struct i2c_slave_op slave3 = {
	.addr = 0x17,	/* mcu common slave address */
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
	.priv = &mcu3_ctx,
};

void mcu_init(void)
{
	assert(sizeof(I2C_REGS) == 0x64);
	mcu1_ctx.map = &mcu_reg;
	mcu3_ctx.map = &mcu_reg;
	mcu1_ctx.setrtc = 0;
	mcu3_ctx.setrtc = 0;
	uptime = 0;

	if (i2c_regs.vender == VENDER_SA5) {
		i2c_slave_register(i2c_ctx1, &slave);
	}
	i2c_slave_register(i2c_ctx3, &slave3);
}


