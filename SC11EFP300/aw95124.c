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

struct PGLine {
	uint8_t port;
	uint8_t bit;
	int active_high;
	const char* name;
};

static uint8_t aw95124_channel[1] = {4};

#if 0
void aw95124_init(void)
{
	/* select channel 5 of i2c siwtch */
	pca9848_set(PCA9848_1, 1 << aw95124_channel[0]);

	/* config port 0,1,2 as input*/
	uint8_t cfg_input = 0xff;
	if(!i2c_master_smbus_write_byte(I2C1, AW95124_SLAVE, 1, CFG0_REGISTER, cfg_input)) dbg_printf("write cfg0 failed\n");
	if(!i2c_master_smbus_write_byte(I2C1, AW95124_SLAVE, 1, CFG1_REGISTER, cfg_input)) dbg_printf("write cfg1 failed\n");
	if(!i2c_master_smbus_write_byte(I2C1, AW95124_SLAVE, 1, CFG2_REGISTER, cfg_input)) dbg_printf("write cfg2 failed\n");
}
#endif


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

static int pg_is_good(uint32_t value, const struct PGLine *pg)
{
	int pg_active;

	pg_active = (value >> (pg->port*8 + pg->bit)) & 0x1;
	return pg->active_high ? pg_active : !pg_active;
}

void check_pg_node(void)
{
	struct PGLine pgl[] = {
		{.port = 0, .bit = 0, .active_high = true, .name = "PG_DDR_VDD2H_L_BM0"},
		{.port = 0, .bit = 1, .active_high = true, .name = "PG_DDR_VDD2H_R_BM0"},
		{.port = 0, .bit = 2, .active_high = true, .name = "PG_DDR_VDDQ_L_BM0"},
		{.port = 0, .bit = 3, .active_high = true, .name = "PG_VP_PCIE_BM0"},
		{.port = 0, .bit = 4, .active_high = true, .name = "PG_DDR_VDD2H_L_BM1"},
		{.port = 0, .bit = 5, .active_high = true, .name = "PG_DDR_VDD2H_R_BM1"},
		{.port = 0, .bit = 6, .active_high = true, .name = "PG_DDR_VDDQ_L_BM1"},
		{.port = 0, .bit = 7, .active_high = true, .name = "PG_DDR_VDDQ_L_BM1"},
		{.port = 1, .bit = 0, .active_high = true, .name = "PG_DDR_VDDQ_R_BM2"},
		{.port = 1, .bit = 1, .active_high = true, .name = "PG_VP_PCIE_BM1"},
		{.port = 1, .bit = 2, .active_high = true, .name = "PG_DDR_VDDQ_L_BM3"},
		{.port = 1, .bit = 3, .active_high = true, .name = "PG_DDR_VDDQ_R_BM3"},
		{.port = 1, .bit = 4, .active_high = true, .name = "PG_DDR_VDD2H_R_BM2"},
		{.port = 1, .bit = 5, .active_high = true, .name = "PG_DDR_VDDQ_L_BM2"},
		{.port = 1, .bit = 6, .active_high = true, .name = "PG_DDR_VDD2H_L_BM2"},
		{.port = 1, .bit = 7, .active_high = true, .name = "PG_DDR_VDD2H_R_BM3"},
		{.port = 2, .bit = 0, .active_high = true, .name = "PG_DDR_VDD2H_L_BM3"},
		{.port = 2, .bit = 1, .active_high = true, .name = "PG_DDR_VDDQ_R_BM0"},
		{.port = 2, .bit = 2, .active_high = true, .name = "PG_VP_PCIE_BM2"},
		{.port = 2, .bit = 3, .active_high = true, .name = "PG_VP_PCIE_BM3"},
	};

	int i;
	uint32_t value;
	if(aw95124_read_inputs(I2C1, &value)){
		for(i = 0; i < ARRAY_SIZE(pgl); i++){
			int good = pg_is_good(value, &pgl[i]);
			dbg_printf("%s : %s (raw=%d)\n", pgl[i].name, good?"Good":"Bad",
				   (value >> (pgl[i].port*8 + pgl[i].bit)) & 1);
		}
	}else{
		dbg_printf("Read aw95124 input register failed\n");
	}
}

//void check_aw95124_slave(void)
//{
//	pca9848_set(PCA9848_1, 1 << 5);
//	uint8_t addr;
//	for(addr = 0x08; addr <= 0x77; ++addr){
//		if(i2c_master_smbus_write_byte(I2C1,addr,1,0,0)){
//			dbg_printf("Find device at 0x%02x\n",addr);
//		}
//	}
//
//	dbg_printf("I2c master scan done\n");
//}
