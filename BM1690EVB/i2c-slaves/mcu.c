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
#include <loop.h>
#include <board_power_impl.h>

#define REG_BOARD_TYPE		0x00
#define REG_SW_VER		0x01
#define REG_HW_VER		0x02
#define REG_CMD		0x03
#define REG_SG2042_TMP		0x04
#define REG_BOARD_TMP		0x05
#define REG_INT_STATUS1		0x06
#define REG_INT_STATUS2		0x07
#define REG_INT_MASK1		0x08
#define REG_INT_MASK2 		0x09

#define REG_SOC_RST_TIMES		0x0a
#define REG_UPTIME_LO		0x0b
#define REG_UPTIME_HI		0x0c
#define REG_POWEROFF_REASON		0x0d

#define RTC_SECONDS		0x0e
#define RTC_MINUTES		0x0f
#define RTC_HOURS		0x10
#define RTC_DATE		0x11
#define RTC_MONTH		0x12
#define RTC_YEAR		0x13

#define REG_MCU_FAMILY		0x18

#define SG2042_TMP_OVER_REPORT			1<<0
#define POWER_68127_TMP_OVER_REPORT		1<<1
#define BOARD_TMP_OVER_REPORT			1<<2
#define BOARD_TMP_OVER_POWEROFF			1<<3
#define SG2042_TMP_OVER_POWEROFF		1<<4
#define SYS_POWER_EXCEPTION_POWEROFF	1<<5
#define V12_EXCEPTION_POWEROFF			1<<6
#define SG2042_REBOOT_CMD				1<<7

#define WATCH_DOG 						1<<0
#define TEST_INT						1<<1

#define I_12V_ATX_L		0x28
#define REG_I_5V_H		0x29
#define REG_I_5V_L		0x30
#define REG_I_DDR_VDD_0V8_H		0x31
#define REG_I_DDR_VDD_0V8_L		0x32
#define REG_I_DDR01_VDDQ_1V2_H		0x33
#define REG_I_DDR01_VDDQ_1V2_L		0x34
#define REG_I_DDR23_VDDQ_1V2_H		0x35
#define REG_I_DDR23_VDDQ_1V2_L		0x36
#define REG_I_VDD_12V_H		0x37
#define REG_I_VDD_12V_L		0x38
#define REG_I_VDD_EMMC_1V8_H		0x39
#define REG_I_VDD_EMMC_1V8_L		0x40
#define REG_I_VDD_EMMC_3V3_H		0x41
#define REG_I_VDD_EMMC_3V3_L		0x42
#define REG_I_VDD_PCIE_C_0V8_H		0x43
#define REG_I_VDD_PCIE_C_0V8_L		0x44
#define REG_I_VDD_PCIE_D_0V8_H		0x45
#define REG_I_VDD_PCIE_D_0V8_L		0x46
#define REG_I_VDD_PCIE_H_1V8_H		0x47
#define REG_I_VDD_PCIE_H_1V8_L		0x48
#define REG_I_VDD_PLL_0V8_H		0x49
#define REG_I_VDD_PLL_0V8_L		0x50
#define REG_I_VDD_RGMII_1V8_H		0x51
#define REG_I_VDD_RGMII_1V8_L		0x52
#define REG_I_VDDC_H		0x53
#define REG_I_VDDC_L		0x54
#define REG_I_VDDIO18_H		0x55
#define REG_I_VDDIO18_L		0x56
#define REG_I_VQPS18_H		0x57
#define REG_I_VQPS18_L		0x58
#define REG_CRITICAL_ACTIONS		0x65
#define REG_CTRITICAL_TEMP		0x66
#define REG_REPOWERON_TEMP		0x67

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
#define FILTER_DEPTH_SHIFT	2
#define FILTER_DEPTH		(1 << FILTER_DEPTH_SHIFT)
#define FILTER_DEPTH_MASK	(FILTER_DEPTH - 1)

struct filter {
	unsigned short data[FILTER_DEPTH];
	unsigned long total;
	unsigned long value;
	int p;
};

static struct filter adc_averge_tab[16];

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

int is_print_enabled;
static struct mcu_ctx mcu_ctx;
static unsigned long last_time_collect;
static unsigned long last_time_output;

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
	case REG_CRITICAL_ACTIONS:
		ctx->critical_action = data;
		break;
	case REG_REPOWERON_TEMP:
		ctx->repoweron_temp = data;
		break;
	case REG_CTRITICAL_TEMP:
		ctx->critical_temp = data;
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
		ret = get_board_type();
		break;
	case REG_SW_VER:
		ret = get_firmware_version();
		break;
	case REG_HW_VER:
		ret = get_pcb_version();
		break;
	case REG_CMD:
		ret = 0;
		break;
	case REG_SG2042_TMP:
		ret = get_soc_temp();
		break;
	case REG_BOARD_TMP:
		ret = get_board_temp();
		break;
	case REG_INT_STATUS1:
		ret = ctx->int_status[0];
		break;
	case REG_INT_STATUS2:
		ret = ctx->int_status[1];
		break;
	case REG_INT_MASK1:
		ret = ctx->int_mask[0];
		break;
	case REG_INT_MASK2:
		ret = ctx->int_mask[1];
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
	case REG_POWEROFF_REASON:
		ret = ctx->poweroff_reason;
	case REG_MCU_FAMILY:
		ret = MCU_FAMILY_GD32E50;
		break;
	case REG_I_5V_H:
		ret = (adc_averge_tab[8].value >> 8);
		break;
	case REG_I_5V_L:
		ret = (adc_averge_tab[8].value & 0xff);
		break;
	case REG_I_DDR_VDD_0V8_H:
		ret = (adc_averge_tab[0].value >> 8);
		break;
	case REG_I_DDR_VDD_0V8_L:
		ret = (adc_averge_tab[0].value & 0xff);
		break;
	case REG_I_DDR01_VDDQ_1V2_H:
		ret = (adc_averge_tab[1].value >> 8);
		break;
	case REG_I_DDR01_VDDQ_1V2_L:
		ret = (adc_averge_tab[1].value & 0xff);
		break;
	case REG_I_DDR23_VDDQ_1V2_H:
		ret = (adc_averge_tab[2].value >> 8);
		break;
	case REG_I_DDR23_VDDQ_1V2_L:
		ret = (adc_averge_tab[2].value & 0xff);
		break;
	case REG_I_VDD_12V_H:
		ret = (adc_averge_tab[9].value >> 8);
		break;
	case REG_I_VDD_12V_L:
		ret = (adc_averge_tab[9].value & 0xff);
		break;
	case REG_I_VDD_EMMC_1V8_H:
		ret = (adc_averge_tab[11].value >> 8);
		break;
	case REG_I_VDD_EMMC_1V8_L:
		ret = (adc_averge_tab[11].value & 0xff);
		break;
	case REG_I_VDD_EMMC_3V3_H:
		ret = (adc_averge_tab[12].value >> 8);
		break;
	case REG_I_VDD_EMMC_3V3_L:
		ret = (adc_averge_tab[12].value & 0xff);
		break;
	case REG_I_VDD_PCIE_C_0V8_H:
		ret = (adc_averge_tab[7].value >> 8);
		break;
	case REG_I_VDD_PCIE_C_0V8_L:
		ret = (adc_averge_tab[7].value & 0xff);
		break;
	case REG_I_VDD_PCIE_D_0V8_H:
		ret = (adc_averge_tab[6].value >> 8);
		break;
	case REG_I_VDD_PCIE_D_0V8_L:
		ret = (adc_averge_tab[6].value & 0xff);
		break;
	case REG_I_VDD_PCIE_H_1V8_H:
		ret = (adc_averge_tab[14].value >> 8);
		break;
	case REG_I_VDD_PCIE_H_1V8_L:
		ret = (adc_averge_tab[14].value & 0xff);
		break;
	case REG_I_VDD_PLL_0V8_H:
		ret = (adc_averge_tab[5].value >> 8);
		break;
	case REG_I_VDD_PLL_0V8_L:
		ret = (adc_averge_tab[5].value & 0xff);
		break;
	case REG_I_VDD_RGMII_1V8_H:
		ret = (adc_averge_tab[10].value >> 8);
		break;
	case REG_I_VDD_RGMII_1V8_L:
		ret = (adc_averge_tab[10].value & 0xff);
		break;
	case REG_I_VDDC_H:
		ret = (adc_averge_tab[13].value >> 8);
		break;
	case REG_I_VDDC_L:
		ret = (adc_averge_tab[13].value & 0xff);
		break;
	case REG_I_VDDIO18_H:
		ret = (adc_averge_tab[15].value >> 8);
		break;
	case REG_I_VDDIO18_L:
		ret = (adc_averge_tab[15].value & 0xff);
		break;
	case REG_I_VQPS18_H:
		ret = (adc_averge_tab[4].value >> 8);
		break;
	case REG_I_VQPS18_L:
		ret = (adc_averge_tab[4].value & 0xff);
		break;
	case REG_CRITICAL_ACTIONS:
		ret = ctx->critical_action;
		break;
	case REG_CTRITICAL_TEMP:
		ret = ctx->critical_temp;
		break;
	case REG_REPOWERON_TEMP:
		ret = ctx->repoweron_temp;
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

static struct i2c_slave_op slave2 = {
	.addr = 0x17,	/* mcu common slave address */
	.mask = 0x00,
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.stop = mcu_stop,
	.reset = mcu_reset,
	.priv = &mcu_ctx,
};

void mcu_x8_init(struct i2c01_slave_ctx *i2c_slave_ctx)
{
	int i;
	mcu_ctx.critical_action = CRITICAL_ACTION_REBOOT;
	mcu_ctx.critical_temp = 105;
	mcu_ctx.repoweron_temp = 60;
	mcu_ctx.poweroff_reason = 0;
	slave.addr = 0x17;
	i2c01_slave_register(i2c_slave_ctx, &slave);

	for(i = 0; i < 16; ++i) {
		filter_init(&adc_averge_tab[i], 0);
	}

	last_time_collect = last_time_output = tick_get();
	loop_add(mcu_process);
}

void mcu_milkv_init(struct i2c_slave_ctx *i2c_slave_ctx)
{
	int i;
	mcu_ctx.critical_action = CRITICAL_ACTION_POWERDOWN;
	mcu_ctx.critical_temp = 105;
	mcu_ctx.repoweron_temp = 60;
	mcu_ctx.poweroff_reason = 0;
	slave.addr = 0x17;
	i2c_slave_register(i2c_slave_ctx, &slave2);

	for(i = 0; i < 16; ++i) {
		filter_init(&adc_averge_tab[i], 0);
	}

	last_time_collect = last_time_output = tick_get();
	loop_add(mcu_process);
}

#define CMD_POWER_OFF		0x02
#define CMD_RESET		0x03	// drag reset pin
#define CMD_REBOOT		0x07	// power off - power on
#define CMD_UPDATE		0x08
extern int power_is_on;

void mcu_process(void)
{
	unsigned long current_time, adc_date;
	int i;

	current_time = tick_get();

	if (current_time - last_time_collect > COLLECT_INTERVAL) {
		for (i = 0; i < 16; ++i) {
			adc_date = adc_read((unsigned long)i);
			filter_in(&adc_averge_tab[i], adc_date);
		}
		last_time_collect = current_time;
	}

	if (is_print_enabled == 1) {
		if(current_time - last_time_output > OUTPUT_CURRENT_INTERVAL) {
			current_print_func();
			last_time_output = current_time;
		}
	}

	if (mcu_ctx.cmd == 0)
		return;

	i2c_disable(I2C0);
	i2c_disable(I2C2);
	nvic_disable_irq(I2C0_EV_IRQn);
	nvic_disable_irq(I2C2_EV_IRQn);
	switch (mcu_ctx.cmd) {
	case CMD_POWER_OFF:
		power_off();
		if (get_board_type() == SG2042EVB)
			if (is_evb_power_key_on() == true)
				power_is_on = true;
		timer_mdelay(500);
		mcu_ctx.poweroff_reason = POWER_OFF_REASON_POWER_OFF;
		break;
	case CMD_RESET:
		chip_reset();
		mcu_ctx.poweroff_reason = POWER_OFF_REASON_RESET;
		break;
	case CMD_REBOOT:
		chip_popd_reset_early();
		if (get_board_type() == SG2042EVB)
			if (is_evb_power_key_on() == true)
				power_is_on = true;
		set_needpoweron();
		mcu_ctx.poweroff_reason = POWER_OFF_REASON_REBOOT;
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

void current_print_func(void)
{
	int V_5V = adc_averge_tab[8].value * 3300 / 4096;
	int V_DDR_VDD_0V8 = adc_averge_tab[0].value * 3300 / 4096;
	int V_DDR01_VDDQ_1V2 = adc_averge_tab[1].value * 3300 / 4096;
	int V_DDR23_VDDQ_1V2 = adc_averge_tab[2].value * 3300 / 4096;
	int V_VDD_12V = adc_averge_tab[9].value * 3300 / 4096;
	int V_VDD_EMMC_1V8 = adc_averge_tab[11].value * 3300 / 4096;
	int V_VDD_EMMC_3V3 = adc_averge_tab[12].value * 3300 / 4096;
	int V_VDD_PCIE_C_0V8 = adc_averge_tab[7].value * 3300 / 4096;
	int V_VDD_PCIE_D_0V8 = adc_averge_tab[6].value * 3300 / 4096;
	int V_VDD_PCIE_H_1V8 = adc_averge_tab[14].value * 3300 / 4096;
	int V_VDD_PLL_0V8 = adc_averge_tab[5].value * 3300 / 4096;
	int V_VDD_RGMII_1V8 = adc_averge_tab[10].value * 3300 / 4096;
	int V_VDDC = adc_averge_tab[13].value * 3300 / 4096;
	int V_VDDIO18 = adc_averge_tab[15].value * 3300 / 4096;
	int V_VQPS18 = adc_averge_tab[4].value * 3300 / 4096;
	printf("V_5V = %d(mV)\n", V_5V);
	printf("V_DDR_VDD_0V8 = %d(mV)\n", V_DDR_VDD_0V8);
	printf("V_DDR01_VDDQ_1V2 = %d(mV)\n", V_DDR01_VDDQ_1V2);
	printf("V_DDR23_VDDQ_1V2 = %d(mV)\n", V_DDR23_VDDQ_1V2);
	printf("V_VDD_12V = %d(mV)\n", V_VDD_12V);
	printf("V_VDD_EMMC_1V8 = %d(mV)\n", V_VDD_EMMC_1V8);
	printf("V_VDD_EMMC_3V3 = %d(mV)\n", V_VDD_EMMC_3V3);
	printf("V_VDD_PCIE_C_0V8 = %d(mV)\n", V_VDD_PCIE_C_0V8);
	printf("V_VDD_PCIE_D_0V8 = %d(mV)\n", V_VDD_PCIE_D_0V8);
	printf("V_VDD_PCIE_H_1V8 = %d(mV)\n", V_VDD_PCIE_H_1V8);
	printf("V_VDD_PLL_0V8 = %d(mV)\n", V_VDD_PLL_0V8);
	printf("V_VDD_RGMII_1V8 = %d(mV)\n", V_VDD_RGMII_1V8);
	printf("V_VDDC = %d(mV)\n", V_VDDC);
	printf("V_VDDIO18 = %d(mV)\n", V_VDDIO18);
	printf("V_VQPS18 = %d(mV)\n", V_VQPS18);
	printf("temp:soc:%d Cel board:%d Cel\n", get_soc_temp(), get_board_temp());
}

uint8_t get_critical_action(void)
{
	return mcu_ctx.critical_action;
}

uint8_t get_critical_temp(void)
{
	return mcu_ctx.critical_temp;
}

uint8_t get_repoweron_temp(void)
{
	return mcu_ctx.repoweron_temp;
}
