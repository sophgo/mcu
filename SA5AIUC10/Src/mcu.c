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
#include "rtc.h"
//uint32_t addr_debug = 0x08080010;
//extern void EEPROM_Write(uint32_t Addr, uint32_t writeFlashData);

static struct mcu_ctx {
	int set_idx;
	int idx;
	int setrtc;
	unsigned short uptime;
	I2C_REGS map;
} mcu_ctx;


#define MCU_REG_MAX REG_NUMBER
// #define MCU_REG_CTL 3

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

static inline uint16_t eeprom_offset(void)
{
	uint16_t offset_base = mcu_ctx.map.eeprom_offset_l |
		(mcu_ctx.map.eeprom_offset_h << 8);
	uint16_t offset_off =
		mcu_ctx.idx - REG_EEPROM_DATA;
	return offset_base + offset_off;
}
void mcu_tick_isr()
{
	mcu_ctx.uptime++;
	i2c_regs.uptime0 = mcu_ctx.uptime & 0xFF;
	i2c_regs.uptime1 = ( mcu_ctx.uptime >> 8 )& 0xFF;
}

static void mcu_write(volatile uint8_t data)
{
	int offset;

	if (mcu_ctx.set_idx) {
		mcu_ctx.idx = (int)data % sizeof(I2C_REGS);
		mcu_ctx.set_idx = 0;
		return;
	}
	*((uint8_t *)(&mcu_ctx.map) + mcu_ctx.idx) = data;


	switch (mcu_ctx.idx) {
	case REG_CMD_REG:
		if (i2c_regs.cmd_reg == 0)
			i2c_regs.cmd_reg = data;
		i2c_regs.cmd_reg_bkup = data;

		if (CPLD_CLR_ERR | i2c_regs.cmd_reg) {
			HAL_GPIO_WritePin(MCU_CPLD_ERR_GPIO_Port, MCU_CPLD_ERR_Pin, GPIO_PIN_RESET);
		}
		break;
	case REG_INTR_STATUS2:
		i2c_regs.intr_status2 = data;
		if (CPLD_SET_ERR | i2c_regs.intr_status2) {
			HAL_GPIO_WritePin(MCU_CPLD_ERR_GPIO_Port, MCU_CPLD_ERR_Pin, GPIO_PIN_SET);
		}
		break;
	case REG_SYS_RTC_YEAR ... (REG_SYS_RTC_YEAR + 5):
		offset = mcu_ctx.idx - REG_SYS_RTC_YEAR;
		((uint8_t *)&mcu_ctx.map.rtc)[offset] = data;
		mcu_ctx.setrtc = 1;
		break;
	case REG_CMD:
		mcu_ctx.map.cmd = data;
		break;
	case REG_EEPROM_OFFSET_L:
		mcu_ctx.map.eeprom_offset_l = data;
		break;
	case REG_EEPROM_OFFSET_H:
		mcu_ctx.map.eeprom_offset_h = data;
		break;
	case REG_EEPROM_DATA ...
		(REG_EEPROM_DATA + MCU_EEPROM_DATA_MAX - 1):
		EEPROM_WriteBytes(eeprom_offset(), (uint8_t *)&data, 1);
		break;
	case REG_VENDER_VAL:
		i2c_regs.vender_val = data;
//		EEPROM_WriteBytes(VENDER_Addr, (uint8_t *)&data, 1);
		break;
	default:
		break;
	}

	idx_inc();
}

extern volatile uint8_t power_on_good;
static uint8_t mcu_read(void)
{
	uint8_t ret = 0;
	static RTC_DateTypeDef sDate;
	static RTC_TimeTypeDef sTime;
	int offset;

//	uint8_t tmp = *((uint8_t *)(&mcu_ctx.map) + mcu_ctx.idx);

	switch (mcu_ctx.idx) {
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
		offset = mcu_ctx.idx - REG_SYS_RTC_YEAR;
		//*
		if (offset < 3)
		{
			HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
			mcu_ctx.map.rtc[0] = sDate.Year;
			mcu_ctx.map.rtc[1] = sDate.Month;
			mcu_ctx.map.rtc[2] = sDate.Date;
		}
		else
		{
			HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
			mcu_ctx.map.rtc[3] = sTime.Hours;
			mcu_ctx.map.rtc[4] = sTime.Minutes;
			mcu_ctx.map.rtc[5] = sTime.Seconds;
			
			
		}
	//*/
		ret = ((uint8_t *)&mcu_ctx.map.rtc)[offset];
		break;
	case REG_CMD:
		ret = mcu_ctx.map.cmd;
		break;
	case REG_DDR:
		ret = i2c_regs.ddr;
		break;
	case REG_PWR_GOOD:
		ret = i2c_regs.power_good;
		break;
	case REG_EEPROM_OFFSET_L:
		ret = mcu_ctx.map.eeprom_offset_l;
		break;
	case REG_EEPROM_OFFSET_H:
		ret = mcu_ctx.map.eeprom_offset_h;
		break;
	case REG_EEPROM_DATA ...
		(REG_EEPROM_DATA + MCU_EEPROM_DATA_MAX - 1):
		EEPROM_ReadBytes(eeprom_offset(), &ret, 1);
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
	default:
		ret = 0xff;
		break;
	}

	idx_inc();
	return ret;
}

static void mcu_stop(void)
{
	if (mcu_ctx.setrtc)
	{
		mcu_ctx.setrtc = 0;
		RTC_DateTypeDef sDate;
		RTC_TimeTypeDef sTime;
		sDate.Year		= mcu_ctx.map.rtc[0];
		sDate.Month		= mcu_ctx.map.rtc[1];
		sDate.Date		= mcu_ctx.map.rtc[2];
		sTime.Hours		= mcu_ctx.map.rtc[3];
		sTime.Minutes	= mcu_ctx.map.rtc[4];
		sTime.Seconds	= mcu_ctx.map.rtc[5];

		HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
		HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	}
}

static struct i2c_slave_op slave = {
	.addr = 0x38,	/* mcu common slave address */
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
};

static struct i2c_slave_op slave3 = {
	.addr = 0x17,	/* mcu common slave address */
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
};

void mcu_init(void)
{
	assert(sizeof(I2C_REGS) == 0x60);
	mcu_ctx.setrtc = 0;
	mcu_ctx.uptime = 0;

	if (i2c_regs.vender == VENDER_SA5) {
		i2c_slave_register(&slave,i2c_ctx0);
	}
	i2c_slave_register(&slave3,i2c_ctx3);
}


