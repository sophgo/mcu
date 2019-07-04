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
//uint32_t addr_debug = 0x08080010;
//extern void EEPROM_Write(uint32_t Addr, uint32_t writeFlashData);

static struct mcu_ctx {
	int set_idx;
	int idx;
	I2C_REGS map;
} mcu_ctx;

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

extern volatile int sequence_cnt;

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
		i2c_regs.cmd_reg = data;
//		if (CPLD_CLR_ERR | i2c_regs.cmd_reg) {
//			HAL_GPIO_WritePin(MCU_CPLD_ERR_GPIO_Port, MCU_CPLD_ERR_Pin, GPIO_PIN_RESET);
//		}
		break;
	case REG_INTR_STATUS2:
		i2c_regs.intr_status2 = data;
//		if (CPLD_SET_ERR | i2c_regs.intr_status2) {
//			HAL_GPIO_WritePin(MCU_CPLD_ERR_GPIO_Port, MCU_CPLD_ERR_Pin, GPIO_PIN_SET);
//		}
		break;
	case REG_SYS_RTC_SEC ... (REG_SYS_RTC_SEC + 5):
		offset = mcu_ctx.idx - REG_SYS_RTC_SEC;
		((uint8_t *)&mcu_ctx.map.rtc)[offset] = data;
		break;
	case REG_SN ... (REG_SN + 3):
		i2c_regs.sn[sequence_cnt] = data;
		EEPROM_Write(SN_Addr + sequence_cnt, data);
		break;
	case REG_MAC0 ... (REG_MAC0 + 7):
		i2c_regs.mac0[sequence_cnt] = data;
		EEPROM_Write(MAC0_Addr + sequence_cnt, data);
		break;
	case REG_MAC1 ... (REG_MAC1 + 7):
		i2c_regs.mac1[sequence_cnt] = data;
		EEPROM_Write(MAC1_Addr + sequence_cnt, data);
		break;
	default:
		break;
	}

	mcu_ctx.idx = (mcu_ctx.idx + 1) % sizeof(I2C_REGS);

		idx_inc();
}

static uint8_t mcu_read(void)
{
	uint8_t ret = 0;
	int offset;

	uint8_t tmp = *((uint8_t *)(&mcu_ctx.map) + mcu_ctx.idx);


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
	case REG_INTR_MASK1:
		ret = i2c_regs.intr_mask1;
		break;
	case REG_INTR_MASK2:
		ret = i2c_regs.intr_mask2;
		break;
	case REG_INTR_STATUS1:
		ret = i2c_regs.intr_status1;
		break;
	case REG_INTR_STATUS2:
		ret = i2c_regs.intr_status2;
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
	case REG_SYS_RTC_SEC ... (REG_SYS_RTC_SEC + 5):
		offset = mcu_ctx.idx - REG_SYS_RTC_SEC;
		ret = ((uint8_t *)&mcu_ctx.map.rtc)[offset];
		break;
	case REG_SN ... (REG_SN + 3):
		offset = mcu_ctx.idx - REG_SN;
		ret = ((uint8_t *)&mcu_ctx.map.sn)[offset];
		break;
	case REG_MAC0 ... (REG_MAC0 + 7):
		offset = mcu_ctx.idx - REG_MAC0;
		ret = ((uint8_t *)&mcu_ctx.map.mac0)[offset];
		break;
	case REG_MAC1 ... (REG_MAC1 + 7):
		offset = mcu_ctx.idx - REG_MAC1;
		ret = ((uint8_t *)&mcu_ctx.map.mac1)[offset];
		break;
	case I_12V_ATX:
		ret = i2c_regs.current.i_12v_atx;
		break;
	case I_VDDIO5:
		ret = i2c_regs.current.i_vddio5;
		break;
	case I_VDDIO18:
		ret = i2c_regs.current.i_vddio18;
		break;
	case I_VDDIO33:
		ret = i2c_regs.current.i_vddio33;
		break;
	case I_VDD_PHY:
		ret = i2c_regs.current.i_vdd_phy;
		break;
	case I_VDD_PCIE:
		ret = i2c_regs.current.i_vdd_pcie;
		break;
	case I_VDD_TPU_MEM:
		ret = i2c_regs.current.i_vdd_tpu_mem;
		break;
	case I_DDR_VDDQ:
		ret = i2c_regs.current.i_ddr_vddq;
		break;
	case I_DDR_VDDQLP:
		ret = i2c_regs.current.i_ddr_vddqlp;
		break;
	case I_LDO_PCIE:
		ret = i2c_regs.current.i_ldo_pcie;
		break;
	default:
		ret = i2c_regs.sw_ver;
		break;
	}

	mcu_ctx.idx = (mcu_ctx.idx + 1) % sizeof(I2C_REGS);
	idx_inc();
	return ret;
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
	assert(sizeof(I2C_REGS) == 0x50);
	i2c_slave_register(&slave);
}


