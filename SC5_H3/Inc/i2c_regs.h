/*
 * i2c_regs.h
 *
 *  Created on: 2019Äê11ÔÂ19ÈÕ
 *      Author: taiqiang.cao
 */

#ifndef I2C_REGS_H_
#define I2C_REGS_H_

typedef struct CURRENT_VAL_t
{
	uint16_t i_12v_atx;
	uint16_t i_vddio5;
	uint16_t i_vddio18;
	uint16_t i_vddio33;
	uint16_t i_vdd_phy;
	uint16_t i_vdd_pcie;
	uint16_t i_vdd_tpu_mem;
	uint16_t i_ddr_vddq;
	uint16_t i_ddr_vddqlp;
	uint16_t i_ldo_pcie;
}CURRENT_VAL;

#define MCU_EEPROM_DATA_MAX    (32)

typedef struct I2C_REGS_t
{
	uint8_t vender;
	uint8_t sw_ver;
	uint8_t hw_ver;
	uint8_t cmd_reg;

	uint8_t temp1684;
	uint8_t temp_board;
	uint8_t intr_status1;
	uint8_t intr_status2;

	uint8_t intr_mask1;
	uint8_t intr_mask2;
	uint8_t rst_1684_times;
	uint8_t uptime0;

	uint8_t uptime1;
	uint8_t cause_pwr_down;
	uint8_t rtc[6];

	uint8_t cmd;
	uint8_t ddr;  //0x15,21
	uint8_t power_good;
	uint8_t mode_flag;

	uint8_t reserved1[2];
	uint16_t v_vddio3v3;//0x1a-0x27
	uint16_t v_vddio18;
	uint16_t v_vdd_phy;
	uint16_t v_vdd_pcie;
	uint16_t v_vdd_tpu_mem;
	uint16_t v_ddr_vddq;
	uint16_t v_ddr_vddqlp;
	CURRENT_VAL current;
	volatile uint8_t reserved2[2];
	volatile uint8_t eeprom_offset_l;
	volatile uint8_t eeprom_offset_h;
	volatile uint8_t eeprom_data[MCU_EEPROM_DATA_MAX];
	volatile uint8_t error_line_l; /* log error line during power on */
	volatile uint8_t error_line_h; /* log error line during power on */
	volatile uint8_t error_code; /* last error code */
	volatile uint8_t reserved3;
}I2C_REGS;

extern I2C_REGS i2c_regs;

#define REG_VENDER			0x00
#define REG_SW_VER			0x01
#define REG_HW_VER			0x02
#define REG_CMD_REG			0x03

#define REG_TEMP1684		0X04
#define REG_TEMP_BOARD		0x05
#define REG_INTR_STATUS1	0x06
#define REG_INTR_STATUS2	0x07

#define REG_INTR_MASK1		0x08
#define REG_INTR_MASK2		0x09
#define REG_1684_RST_TIMES	0x0a
#define REG_UPTIME0			0x0b

#define REG_UPTIME1			0x0c
#define REG_CAUSE_PWR_DOWN	0x0d
#define REG_SYS_RTC_SEC		0x0e
#define REG_SYS_RTC_MIN		0x0f

#define REG_SYS_RTC_HOUR	0x10
#define REG_SYS_RTC_DAY		0x11
#define REG_SYS_RTC_MON		0x12
#define REG_SYS_RTC_YEAR	0x13

#define REG_CMD				0x14
#define REG_DDR				0x15
#define REG_POWER_GOOD		0x16
#define REG_MODE_FLAG		0x17
/* some reserved here, original MAC0 and MAC1 */
#define I_12V_ATX_L			0x28
#define I_12V_ATX_H			0x29
#define I_VDDIO5_L			0x2a
#define I_VDDIO5_H			0x2B
#define I_VDDIO18_L			0x2c
#define I_VDDIO18_H			0x2D
#define I_VDDIO33_L			0x2e
#define I_VDDIO33_H			0x2f
#define I_VDD_PHY_L			0x30
#define I_VDD_PHY_H			0x31
#define I_VDD_PCIE_L		0x32
#define I_VDD_PCIE_H		0x33
#define I_VDD_TPU_MEM_L		0x34
#define I_VDD_TPU_MEM_H		0x35
#define I_DDR_VDDQ_L		0x36
#define I_DDR_VDDQ_H		0x37
#define I_DDR_VDDQLP_L		0x38
#define I_DDR_VDDQLP_H		0x39
#define I_LDO_PCIE_L		0x3a
#define I_LDO_PCIE_H		0x3b
#define REG_EEPROM_OFFSET_L	0x3e	/* 16bit eeprom address, low 8bits */
#define REG_EEPROM_OFFSET_H	0x3f	/* 16bit eeprom address, high 8bits */
#define REG_EEPROM_DATA		0x40	/* eeprom data */
#define REG_ERROR_LINE_L	0x60	/* error line low byte */
#define REG_ERROR_LINE_H	0x61	/* error line high byte */
#define REG_ERROR_CODE		0x62	/* error code */
#define REG_I2C2_STATE		0x63	/* i2c2 state */
#define REG_NUMBER		sizeof(I2C_REGS)

#define BIT0   (0X01 << 0)
#define BIT1   (0X01 << 1)
#define BIT2   (0X01 << 2)
#define BIT3   (0X01 << 3)
#define BIT4   (0X01 << 4)
#define BIT5   (0X01 << 5)
#define BIT6   (0X01 << 6)
#define BIT7   (0X01 << 7)

#define BOARD_OVER_TEMP BIT3
#define BM1684_OVER_TEMP BIT4
#define CPLD_CLR_ERR BIT5
#define CPLD_SET_ERR BIT7

#define WDT_RST			BIT1

//CPLD Command
#define CMD_CPLD_PWR_ON			0x01       //1684 power on
#define CMD_CPLD_PWR_DOWN		0x02	   //1684 power down
#define CMD_CPLD_1684RST		0x03       //reset 1684
#define CMD_CPLD_SWRST			0x04	   //soft resetting
#define CMD_CPLD_CLR			0x05       // clean MCU_ERR_INT, set 0.
//BM1684 Command
#define CMD_BM1684_RST			0x06       // power is always on
#define CMD_BM1684_REBOOT		0x07       // power down
#define CMD_MCU_UPDATE			0x08		//MCU UPDATE

//EEPROM  FLASH
#define EEPROM_BASE_ADDR	0x08080C00
#define EEPROM_BANK_SIZE	0x0C00

//
//uint32_t writeFlashData = 0x55aa55aa;
//uint8_t readFlashData[4] = {0};
//uint32_t addr = 0x08080000;
//uint16_t addr_offset = 0x0;
//
/*
 *  SN Addr  : EEPROM_BANK1_START
 *  MAC0 Addr: EEPROM_BANK1_START +  4
 *  MAC1 Addr: EEPROM_BANK1_START + 12
 */
#define SN_Addr		(EEPROM_BASE_ADDR + 32 * 2)
#define MAC0_Addr	(EEPROM_BASE_ADDR + 32 * 0)
#define MAC1_Addr	(EEPROM_BASE_ADDR + 32 * 1)

#define UPDATE_FLAG_OFFSET		(0xbf0)

#define MCU_VERSION 0x00

#define VENDER_EVB		(0x00)
#define VENDER_SA5		(0x01)
#define VENDER_SC5		(0x02)
#define VENDER_SE5		(0x03)
#define VENDER_SM5_P	(0x04)
#define VENDER_SM5_S	(0x05)
#define VENDER_SA6		(0x06)
#define VENDER_SC5_H3	(0x07)

#endif /* I2C_REGS_H_ */
