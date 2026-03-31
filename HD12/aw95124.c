#include <gd32f4xx_i2c.h>
#include <pca9848.h>
#include <i2c_master.h>
#include <system.h>
#include <common.h>

#define AW95124_SLAVE	0x22

/* define port 0 pin definition */
#define PG_DDR_VDD2H_L_BM0	(1 << 0)
#define PG_DDR_VDD2H_R_BM0	(1 << 1)
#define PG_DDR_VDDQ_L_BM0	(1 << 2)
#define PG_VP_PCIE_BM0		(1 << 3)
#define PG_DDR_VDD2H_L_BM1	(1 << 4)
#define PG_DDR_VDD2H_R_BM1	(1 << 5)
#define PG_DDR_VDDQ_L_BM1	(1 << 6)
#define PG_DDR_VDDQ_R_BM1	(1 << 7)

/* define port 1 pin definition */
#define PG_DDR_VDDQ_R_BM2	(1 << 0)
#define PG_VP_PCIE_BM1		(1 << 1)
#define PG_DDR_VDDQ_L_BM3	(1 << 2)
#define PG_DDR_VDDQ_R_BM3	(1 << 3)
#define PG_DDR_VDD2H_R_BM2	(1 << 4)
#define PG_DDR_VDDQ_L_BM2	(1 << 5)
#define PG_DDR_VDD2H_L_BM2	(1 << 6)
#define PG_DDR_VDD2H_R_BM3	(1 << 7)

/* define port 0 pin definition */
#define PG_DDR_VDD2H_L_BM3	(1 << 0)
#define PG_DDR_VDDQ_R_BM0	(1 << 1)
#define PG_VP_PCIE_BM2		(1 << 2)
#define PG_VP_PCIE_BM3		(1 << 3)


#define CFG0_REGISTER		0x0C
#define CFG1_REGISTER		0x0D
#define CFG2_REGISTER		0x0E

#define INPUT_IN0		0x00
#define INPUT_IN1		0x01
#define INPUT_IN2		0x02

#if 0
struct PGLine {
	uint8_t port;
	uint8_t bit;
	int active_high;
	const char* name;
};

static uint8_t aw95124_channel[1] = {4};

static int aw95124_read_inputs(int i2c, uint32_t *value)
{
	uint8_t in0,in1,in2;
	/* select channel 5 of i2c siwtch */
	pca9848_set(PCA9848_1, 1 << aw95124_channel[0]);

	/* send a slave_addr + read bit */
	if(i2c_master_smbus_write(I2C1, AW95124_SLAVE, 1, INPUT_IN0) != 0){
		dbg_printf("[aw95124_read_inputs] write INPUT0 failed\n");
		return false;
	}
	if(i2c_master_smbus_read(i2c, AW95124_SLAVE, 1, &in0) != 0){
		dbg_printf("[aw95124_read_inputs] read INPUT0 failed\n");
		return false;
	}

	if(i2c_master_smbus_write(I2C1, AW95124_SLAVE, 1, INPUT_IN1) != 0){
		dbg_printf("[aw95124_read_inputs] write INPUT1 failed\n");
		return false;
	}
	if(i2c_master_smbus_read(i2c, AW95124_SLAVE, 1, &in1) != 0){
		dbg_printf("[aw95124_read_inputs] read INPUT1 failed\n");
		return false;
	}

	if(i2c_master_smbus_write(I2C1, AW95124_SLAVE, 1, INPUT_IN2) != 0){
		dbg_printf("[aw95124_read_inputs] write INPUT2 failed\n");
		return false;
	}
	if(i2c_master_smbus_read(i2c, AW95124_SLAVE, 1, &in2) != 0){
		dbg_printf("[aw95124_read_inputs] read INPUT2 failed\n");
		return false;
	}

	in2 &= 0xf;
	*value = ((uint32_t)in2 << 16) | ((uint32_t)in1 << 8) | (uint32_t)in0;
	return true;
}
#endif
