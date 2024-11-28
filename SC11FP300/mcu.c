#include <gd32e50x_i2c.h>
#include <mcu.h>
#include <adc.h>
#include <stdio.h>
#include <i2c_slave.h>
#include <i2c01_slave.h>
#include <slave.h>
#include <string.h>
#include <tick.h>
#include <timer.h>
#include <debug.h>
#include <chip.h>
#include <common.h>
#include <power.h>
#include <project.h>
#include <upgrade.h>
#include <mon.h>
#include <system.h>
#include <isl68224.h>
#include <dvfs.h>

#define REG_BOARD_TYPE		0x00
#define REG_SW_VER		0x01
#define REG_HW_VER		0x02
#define REG_CMD			0x03
#define REG_BM1690_1_TMP	0x04
#define REG_BOARD_1_TMP		0x05
#define REG_BM1690_2_TMP	0x06
#define REG_BOARD_2_TMP		0x07
#define REG_VDDR_VOLT_0_L	0x08
#define REG_VDDR_VOLT_0_H	0x09
#define REG_VDDR_VOLT_1_L	0x0a
#define REG_VDDR_VOLT_1_H	0x0b
#define REG_I12V_L		0x0c
#define REG_I12V_H		0x0d
#define REG_SYS_I12V_L		0x0e
#define REG_SYS_I12V_H		0x0f
#define REG_PCIE_I12V_L		0x10
#define REG_PCIE_I12V_H		0x11
#define REG_PCIE_I3V3_L		0x12
#define REG_PCIE_I3V3_H		0x13


#define REG_MCU_FAMILY		0x18
#define REG_DVFS_ENABLE		0X19
#define REG_POWER_LIMIT		0x1a

#define BM1690_TMP_OVER_REPORT			1<<0
#define POWER_68127_TMP_OVER_REPORT		1<<1
#define BOARD_TMP_OVER_REPORT			1<<2
#define BOARD_TMP_OVER_POWEROFF			1<<3
#define BM1690_TMP_OVER_POWEROFF		1<<4
#define SYS_POWER_EXCEPTION_POWEROFF		1<<5
#define V12_EXCEPTION_POWEROFF			1<<6
#define BM1690_REBOOT_CMD			1<<7

#define WATCH_DOG 				1<<0
#define TEST_INT				1<<1

#define REG_SET_DROOP_L		0x50
#define REG_SET_DROOP_H		0x51
#define REG_DROOP_0_L		0x58
#define REG_DROOP_0_H		0x59
#define REG_DROOP_1_L		0x5a
#define REG_DROOP_1_H		0x5b

#define REG_SET_VOLT_L		0x60
#define REG_SET_VOLT_H		0x61
#define REG_FLASH_CMD		0x63
#define REG_FLASH_OFFSET	0x7c
#define REG_FLASH_DATA		0x80
#define REG_FLASH_FLUSH		0xff

#define FLASH_CMD_UNLOCK	0x02
#define FLASH_CMD_LOCK		0x03
#define FLASH_CMD_ERASE		0x04

#define MCU_REG_MAX		0x100

#define COLLECT_INTERVAL	25
#define OUTPUT_CURRENT_INTERVAL	1000
#define FILTER_DEPTH_SHIFT	5
#define FILTER_DEPTH		(1 << FILTER_DEPTH_SHIFT)
#define FILTER_DEPTH_MASK	(FILTER_DEPTH - 1)

struct filter {
	unsigned short data[FILTER_DEPTH];
	unsigned long total;
	unsigned long value;
	int p;
};

static unsigned long last_time_collect;
static struct filter adc_averge_tab[4];
static unsigned char set_vddr_val[2];
static uint8_t vddr_volt[2][2];
static unsigned char set_droop_val[2];
static uint8_t droop_val[2][2];
extern uint8_t dvfs_p_enable;
extern uint8_t atx_300W;

static unsigned long filter_init(struct filter *f, unsigned long d)
{
	int i;

	f->p = 0;
	f->total = 0;
	for (i = 0; i < FILTER_DEPTH; ++i) {
		f->data[i] = d;
		f->total += d;
	}
	return d;
}

static unsigned long filter_in(struct filter *f, unsigned long d)
{
	f->total -= f->data[f->p];
	f->total += d;
	f->data[f->p] = d;
	f->p = (f->p + 1) & FILTER_DEPTH_MASK;
	f->value = f->total >> FILTER_DEPTH_SHIFT;

#ifdef FILTER_DISABLE
	return d;
#else
	return f->value;
#endif
}

/* unit: W*/
unsigned int get_12v_power(void)
{
	return adc_averge_tab[0].value * 12 / 1000 ;
}

/* unit: W*/
unsigned char get_12v_power_l(void)
{
	unsigned int power;
	power = get_12v_power();
	return power & 0xff;
}

/* unit: W*/
unsigned char get_12v_power_h(void)
{
	unsigned int power;
	power = get_12v_power();
	return power >> 8;
}

struct mcu_ctx {
	int set_idx;
	int idx;
	int cmd_tmp;
	uint8_t cmd;
	uint16_t tmp;
	uint8_t int_status[2];
	uint8_t int_mask[2];
	uint8_t critical_action;
	uint8_t	repoweron_temp;
	uint8_t critical_temp;
	uint8_t poweroff_reason;
	uint8_t __attribute__((aligned(4))) flash_offset[4];
	uint8_t __attribute__((aligned(4))) flash_data[128];
	int flash_flush;
};

static struct mcu_ctx mcu_ctx;

static inline void idx_set(struct mcu_ctx *ctx, uint8_t idx)
{
	ctx->idx = idx % MCU_REG_MAX;
}

static inline void idx_inc(struct mcu_ctx *ctx)
{
	ctx->idx = (ctx->idx + 1) % MCU_REG_MAX;
}

static inline uint32_t flash_byte2u32(void *byte)
{
	uint8_t *p;

	p = byte;
	/* big endian */
	return p[3] | (p[2] << 8) | (p[1] << 16) | (p[0] << 24);
}

static inline uint16_t  byte2u16(void *byte)
{
	uint8_t *p;

	p = byte;

	return p[1] << 8 | p[0] ;
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

static void mcu_match(void *priv, int dir)
{
	struct mcu_ctx *ctx = priv;

	if (dir == I2C_SLAVE_WRITE)
		ctx->set_idx = 1;
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
	case REG_SET_VOLT_L:
		set_vddr_val[0] = data;
		break;
	case REG_SET_VOLT_H:
		set_vddr_val[1] = data;
		break;
	case REG_DVFS_ENABLE:
		dvfs_p_enable = data;
		break;
	case REG_POWER_LIMIT:
		atx_300W = data;
		dvfs_p_threshold();
		break;
	case REG_SET_DROOP_L:
		set_droop_val[0] = data;
		break;
	case REG_SET_DROOP_H:
		set_droop_val[1] = data;
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
		ret = SC11FP300;
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
	case REG_BM1690_1_TMP:
		ret = get_soc_temp(0);
		break;
	case REG_BOARD_1_TMP:
		ret = get_board_temp(0);
		break;
	case REG_BM1690_2_TMP:
		ret = get_soc_temp(1);
		break;
	case REG_BOARD_2_TMP:
		ret = get_board_temp(1);
		break;
	case REG_VDDR_VOLT_0_L:
		ret = vddr_volt[0][1];
		break;
	case REG_VDDR_VOLT_0_H:
		ret = vddr_volt[0][0];
		break;
	case REG_VDDR_VOLT_1_L:
		ret = vddr_volt[1][1];
		break;
	case REG_VDDR_VOLT_1_H:
		ret = vddr_volt[1][0];
		break;
	case REG_I12V_L:
		ret = (adc_averge_tab[0].value & 0xff);
		break;
	case REG_I12V_H:
		ret = (adc_averge_tab[0].value >> 8);
		break;
	case REG_SYS_I12V_L:
		ret = (adc_averge_tab[1].value & 0xff);
		break;
	case REG_SYS_I12V_H:
		ret = (adc_averge_tab[1].value >> 8);
		break;
	case REG_PCIE_I12V_L:
		ret = (adc_averge_tab[2].value & 0xff);
		break;
	case REG_PCIE_I12V_H:
		ret = (adc_averge_tab[2].value >> 8);
		break;
	case REG_PCIE_I3V3_L:
		ret = (adc_averge_tab[3].value & 0xff);
		break;
	case REG_PCIE_I3V3_H:
		ret = (adc_averge_tab[3].value >> 8);
		break;
	case REG_MCU_FAMILY:
		ret = MCU_FAMILY_GD32E50;
		break;
	case REG_DVFS_ENABLE:
		ret = dvfs_p_enable;
		break;
	case REG_POWER_LIMIT:
		ret = atx_300W;
		break;
	case REG_SET_DROOP_L:
		ret = set_droop_val[0];
		break;
	case REG_SET_DROOP_H:
		ret = set_droop_val[1];
		break;
	case REG_DROOP_0_L:
		ret = droop_val[0][1];
		break;
	case REG_DROOP_0_H:
		ret = droop_val[0][0];
		break;
	case REG_DROOP_1_L:
		ret = droop_val[1][1];
		break;
	case REG_DROOP_1_H:
		ret = droop_val[1][0];
		break;
	case REG_SET_VOLT_L:
		ret = set_vddr_val[0];
		break;
	case REG_SET_VOLT_H:
		ret = set_vddr_val[1];
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

static struct i2c01_slave_op slave = {
	.addr = 0x17,	/* mcu common slave address */
	.mask = 0x00,
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
	.reset = mcu_reset,
	.priv = &mcu_ctx,
};

void mcu_init(struct i2c01_slave_ctx *i2c_slave_ctx)
{
	int i;

	mcu_ctx.critical_action = CRITICAL_ACTION_REBOOT;
	mcu_ctx.critical_temp = 105;
	mcu_ctx.repoweron_temp = 60;
	mcu_ctx.poweroff_reason = 0;
	slave.addr = 0x17;
	i2c01_slave_register(i2c_slave_ctx, &slave);

	for(i = 0; i < 4; ++i) {
		filter_init(&adc_averge_tab[i], 0);
	}

	last_time_collect = tick_get();
}

#define CMD_CHIP0_VDDR		0x01
#define CMD_CHIP1_VDDR		0x02
#define CMD_CHIP0_DROOP		0x11
#define CMD_CHIP1_DROOP		0x12
#define CMD_REBOOT		0x07
#define CMD_UPDATE		0x08
extern int power_is_on;
static int flag = 0;
void mcu_process(void)
{
	unsigned long current_time, adc_data, temp_data;
	unsigned short temp;
	int i;


	current_time = tick_get();

	if (current_time - last_time_collect > COLLECT_INTERVAL) {
		adc_data = adc_read_i12v();
		filter_in(&adc_averge_tab[0], adc_data);
		adc_data =  get_i12v_atx();
		filter_in(&adc_averge_tab[1], adc_data);
		adc_data =  get_i12v_pcie();
		filter_in(&adc_averge_tab[2], adc_data);
		adc_data =  get_i3v3_pcie();
		filter_in(&adc_averge_tab[3], adc_data);
		temp_data = isl68224_output_voltage(0, 0);
		vddr_volt[0][0] = (temp_data >> 8) & 0xff;
		vddr_volt[0][1] = temp_data & 0xff;
		temp_data = isl68224_output_voltage(1, 0);
		vddr_volt[1][0] = (temp_data >> 8) & 0xff;
		vddr_volt[1][1] = temp_data & 0xff;
		temp_data = isl68224_out_droop(0, 0);
		droop_val[0][0] = (temp_data >> 8) & 0xff;
		droop_val[0][1] = temp_data & 0xff;
		temp_data = isl68224_out_droop(1, 0);
		droop_val[1][0] = (temp_data >> 8) & 0xff;
		droop_val[1][1] = temp_data & 0xff;
		last_time_collect = current_time;
	}
	if (mcu_ctx.cmd == 0)
		return;

	i2c_disable(I2C0);
	i2c_disable(I2C2);
	nvic_disable_irq(I2C0_EV_IRQn);
	nvic_disable_irq(I2C2_EV_IRQn);
	switch (mcu_ctx.cmd) {
	case CMD_CHIP0_VDDR:
		temp = byte2u16(set_vddr_val);
		isl68224_set_out_voltage(0, 0, (int)temp);
		temp_data = isl68224_output_voltage(0, 0);
		vddr_volt[0][0] = (temp_data >> 8) & 0xff;
		vddr_volt[0][1] = temp_data & 0xff;
		break;
	case CMD_CHIP1_VDDR:
		temp = byte2u16(set_vddr_val);
		isl68224_set_out_voltage(1, 0, (int)temp);
		temp_data = isl68224_output_voltage(1, 0);
		vddr_volt[0][0] = (temp_data >> 8) & 0xff;
		vddr_volt[0][1] = temp_data & 0xff;
		break;
	case CMD_CHIP0_DROOP:
		temp = byte2u16(set_droop_val);
		isl68224_set_out_voltage(0, 0, (int)temp);
		break;
	case CMD_CHIP1_DROOP:
		temp = byte2u16(set_droop_val);
		isl68224_set_out_voltage(1, 0, (int)temp);
		break;
	case CMD_UPDATE:
		nvic_enable_irq(I2C0_EV_IRQn);
		nvic_enable_irq(I2C2_EV_IRQn);
		i2c_upgrade_start();
		break;
	default:
		break;
	}

	mcu_ctx.cmd = 0;
	mcu_ctx.cmd_tmp = 0;
	i2c_enable(I2C0);
	i2c_enable(I2C2);
	slave_init();
}
