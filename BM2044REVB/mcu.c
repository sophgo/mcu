#include <mcu.h>
#include <adc.h>
#include <stdio.h>
#include <i2c01_slave.h>
#include <string.h>
#include <tick.h>
#include <timer.h>

#define REG_BOARD_TYPE		0x00
#define REG_SW_VER		0x01
#define REG_HW_VER		0x02
#define REG_CMD		0x03
#define REG_BM2044R_TMP		0x04
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

#define SG2042R_TMP_OVER_REPORT			1<<0
#define POWER_68127_TMP_OVER_REPORT		1<<1
#define BOARD_TMP_OVER_REPORT			1<<2
#define BOARD_TMP_OVER_POWEROFF			1<<3
#define SG2042R_TMP_OVER_POWEROFF		1<<4
#define SYS_POWER_EXCEPTION_POWEROFF	1<<5
#define V12_EXCEPTION_POWEROFF			1<<6
#define SG2042R_REBOOT_CMD				1<<7

#define WATCH_DOG 						1<<0
#define TEST_INT						1<<1

#define I_12V_ATX_L		0x28
#define REG_I_DDR_VDD_0V8_H		0x29
#define REG_I_DDR_VDD_0V8_L		0x30
#define REG_I_DDR01_VDDQ_1V2_H		0x31
#define REG_I_DDR01_VDDQ_1V2_L		0x32
#define REG_I_VDD_EMMC_1V8_H		0x33
#define REG_I_VDD_EMMC_1V8_L		0x34
#define REG_I_VDD_EMMC_3V3_H		0x35
#define REG_I_VDD_EMMC_3V3_L		0x36
#define REG_I_VDD_PLL_0V8_H		0x37
#define REG_I_VDD_PLL_0V8_L		0x38
#define REG_I_DDR01_VPP_2V5_H		0x39
#define REG_I_DDR01_VPP_2V5_L		0x40
#define REG_I_DDR01_VTT_0V6_H		0x41
#define REG_I_DDR01_VTT_0V6_L		0x42
#define REG_I_VDD_12V_H		0x43
#define REG_I_VDD_12V_L		0x44
#define REG_I_5V_H		0x45
#define REG_I_5V_L		0x46
#define MCU_REG_MAX		0x47

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
	default:
		break;
	}

	idx_inc(ctx);
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
		ret = get_hardware_version();
		break;
	case REG_CMD:
		ret = 0;
		break;
	case REG_BM2044R_TMP:
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
	case REG_I_VDD_EMMC_1V8_H:
		ret = (adc_averge_tab[2].value >> 8);
		break;
	case REG_I_VDD_EMMC_1V8_L:
		ret = (adc_averge_tab[2].value & 0xff);
		break;
	case REG_I_VDD_EMMC_3V3_H:
		ret = (adc_averge_tab[3].value >> 8);
		break;
	case REG_I_VDD_EMMC_3V3_L:
		ret = (adc_averge_tab[3].value & 0xff);
		break;
	case REG_I_VDD_PLL_0V8_H:
		ret = (adc_averge_tab[7].value >> 8);
		break;
	case REG_I_VDD_PLL_0V8_L:
		ret = (adc_averge_tab[7].value & 0xff);
		break;
	case REG_I_DDR01_VPP_2V5_H:
		ret = (adc_averge_tab[8].value >> 8);
		break;
	case REG_I_DDR01_VPP_2V5_L:
		ret = (adc_averge_tab[8].value & 0xff);
		break;
	case REG_I_DDR01_VTT_0V6_H:
		ret = (adc_averge_tab[9].value >> 8);
		break;
	case REG_I_DDR01_VTT_0V6_L:
		ret = (adc_averge_tab[9].value & 0xff);
		break;
	case REG_I_VDD_12V_H:
		ret = (adc_averge_tab[14].value >> 8);
		break;
	case REG_I_VDD_12V_L:
		ret = (adc_averge_tab[14].value & 0xff);
		break;
	case REG_I_5V_H:
		ret = (adc_averge_tab[15].value >> 8);
		break;
	case REG_I_5V_L:
		ret = (adc_averge_tab[15].value & 0xff);
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
	mcu_ctx.critical_action = CRITICAL_ACTION_POWERDOWN;
	mcu_ctx.critical_temp = 120;
	mcu_ctx.repoweron_temp = 85;
	slave.addr = 0x17;
	i2c01_slave_register(i2c_slave_ctx, &slave);

	for(i = 0; i < 16; ++i) {
		filter_init(&adc_averge_tab[i], 0);
	}

	last_time_collect = last_time_output = tick_get();
}

void mcu_process(void)
{
	unsigned long current_time, adc_date;
	int i;

	current_time = tick_get();

	if (current_time - last_time_collect > COLLECT_INTERVAL) {
		for (i = 0; i < 4; ++i) {
			adc_date = adc_read((unsigned long)i);
			filter_in(&adc_averge_tab[i], adc_date);
		}
		for (i = 7; i < 10; ++i) {
			adc_date = adc_read((unsigned long)i);
			filter_in(&adc_averge_tab[i], adc_date);
		}
		for (i = 14; i < 16; ++i) {
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
}

void current_print_func(void)
{
	int V_DDR_VDD_0V8 = adc_averge_tab[0].value * 3300 / 4096;
	int V_DDR01_VDDQ_1V2 = adc_averge_tab[1].value * 3300 / 4096;
	int V_VDD_EMMC_1V8 = adc_averge_tab[2].value * 3300 / 4096;
	int V_VDD_EMMC_3V3 = adc_averge_tab[3].value * 3300 / 4096;
	int V_VDD_PLL_0V8 = adc_averge_tab[7].value * 3300 / 4096;
	int V_DDR01_VPP_2V5 = adc_averge_tab[8].value * 3300 / 4096;
	int V_DDR01_VTT_0V6 = adc_averge_tab[9].value * 3300 / 4096;
	int V_VDD_12V = adc_averge_tab[14].value * 3300 / 4096;
	int V_5V = adc_averge_tab[15].value * 3300 / 4096;

	printf("V_DDR_VDD_0V8 = %d(mV)\n", V_DDR_VDD_0V8);
	printf("V_DDR01_VDDQ_1V2 = %d(mV)\n", V_DDR01_VDDQ_1V2);
	printf("V_VDD_EMMC_1V8 = %d(mV)\n", V_VDD_EMMC_1V8);
	printf("V_VDD_EMMC_3V3 = %d(mV)\n", V_VDD_EMMC_3V3);
	printf("V_VDD_PLL_0V8 = %d(mV)\n", V_VDD_PLL_0V8);
	printf("V_DDR01_VPP_2V5 = %d(mV)\n", V_DDR01_VPP_2V5);
	printf("V_DDR01_VTT_0V6 = %d(mV)\n", V_DDR01_VTT_0V6);
	printf("V_VDD_12V = %d(mV)\n", V_VDD_12V);
	printf("V_5V = %d(mV)\n", V_5V);
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
