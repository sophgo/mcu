/* Out-of-band management */

#include <i2c_slave.h>
#include <timer.h>
#include <common.h>
#include <chip.h>
#include <mon.h>
#include <project.h>
#include <mcu.h>
#include <adc.h>


/* i2c slave operations */

#define BMC_SLAVE_BASE	0x60
#define BMC_SOC_INFO_BASE	0x7010000000ULL
#define BMC_REG_MASK		(0xff)
#define CHIP_MAP_SIZE		0x80

#define BMC_SOC_INFO_SOC_TEMP_OFFSET		0
#define BMC_SOC_INFO_BOARD_TEMP_OFFSET	1
#define BMC_SOC_INFO_BOARD_TYPE		2
#define BMC_SOC_INFO_VERSION_L		3
#define BMC_SOC_INFO_VERSION_H		4
#define BMC_SOC_INFO_12V_POWER_L		5
#define BMC_SOC_INFO_12V_POWER_H		6
#define SN_START_ADDR				7
#define SN_END_ADDR				23
#define BMC_SOC_INDEX			24

#define BMC_SOC_INFO_ADDR(reg)	(BMC_SOC_INFO_BASE +	\
					 BMC_SOC_INFO_ ## reg ## _OFFSET)


struct bmc_i2c_ctx {
	int set_idx;
	uint8_t idx;
	int soc;
} ctx;


static uint8_t get_sn_bit(int i)
{
	uint8_t tmp;
	volatile uint8_t *p_sn = (uint8_t *) SN_BASE;

	tmp = p_sn[i];

	return tmp;

}

static inline void idx_set(uint8_t idx)
{
	ctx.idx = idx & BMC_REG_MASK;
}

static inline void idx_inc(void)
{
	ctx.idx = (ctx.idx + 1) & BMC_REG_MASK;
}

static void bmc_i2c_slave_match(void *priv, uint8_t addr,  int dir)
{
	ctx.soc = addr - BMC_SLAVE_BASE;
	if (dir == I2C_SLAVE_WRITE)
		ctx.set_idx = 1;
}

static void bmc_i2c_slave_write(void *priv, uint8_t data)
{
	if (ctx.set_idx) {
		idx_set(data);
		ctx.set_idx = 0;
		return;
	}

	/* ignore write, just increase index */

	idx_inc();
}

static uint8_t bmc_i2c_slave_read(void *priv)
{
	uint8_t data = 0;
	int soc = ctx.soc % SOC_NUM;

	if (chip_is_enabled()) {
		switch (ctx.idx) {
		case BMC_SOC_INFO_SOC_TEMP_OFFSET:
			data = get_soc_temp(soc);
			break;
		case BMC_SOC_INFO_BOARD_TEMP_OFFSET:
			data = get_board_temp(soc);
			break;
		case BMC_SOC_INFO_BOARD_TYPE:
			data = HD12;
			break;
		case BMC_SOC_INFO_VERSION_L:
			data = get_firmware_version();
			break;
		case BMC_SOC_INFO_VERSION_H:
			data = get_hardware_version();
			break;
		case BMC_SOC_INFO_12V_POWER_L:
			data = get_12v_power_l();
			break;
		case BMC_SOC_INFO_12V_POWER_H:
			data = get_12v_power_h();
			break;
		case SN_START_ADDR ...  SN_END_ADDR:
			data = get_sn_bit(ctx.idx - SN_START_ADDR);
			break;
		case BMC_SOC_INDEX:
			data = soc;
			break;
		default:
			data = 0xff;
			break;
		}
	} else {
		data = (soc << 8) | ctx.idx;
	}

	idx_inc();
	return data;
}

static struct i2c_slave_op slave = {
	.mask = 0x01,
	.match	= bmc_i2c_slave_match,
	.write	= bmc_i2c_slave_write,
	.read	= bmc_i2c_slave_read,
};

void bmc_init(struct i2c_slave_ctx *i2c)
{
	slave.addr = BMC_SLAVE_BASE + get_module_id() * 2;
	i2c_slave_register(i2c, &slave);
}