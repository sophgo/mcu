/*
 * power_seq.c: CV84x6 上电时序控制
 *
 * -u: 相邻引脚之间等待 gap(us) 后依次拉高
 * -d: 6 个引脚立即全部拉低（无间隔）
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_gpio.h>
#include <timer/timer.h>
#include <pin.h>
#include <power_seq/power_seq.h>
#include <debug.h>
#include <string.h>

static int power_is_on;
static unsigned int seq_gaps_us[POWER_SEQ_GAP_NUM];

static const struct {
	uint32_t port;
	uint32_t pin;
	const char *name;
} power_seq_nodes[] = {
	{ EN_12V0_PORT,		EN_12V0_PIN,	"EN_12V0" },
	{ POWEREN1_PORT,	POWEREN1_PIN,	"POWEREN1" },
	{ POWEREN2_PORT,	POWEREN2_PIN,	"POWEREN2" },
	{ POWEREN3_PORT,	POWEREN3_PIN,	"POWEREN3" },
	{ SYS_RSTN_H_PORT,	SYS_RSTN_H_PIN,	"SYS_RSTN_H" },
	{ PWR_RSTN_H_PORT,	PWR_RSTN_H_PIN,	"PWR_RSTN_H" },
};

void power_seq_set_gaps_us(const unsigned int gaps[POWER_SEQ_GAP_NUM])
{
	memcpy(seq_gaps_us, gaps, sizeof(seq_gaps_us));
}

void power_seq_get_gaps_us(unsigned int gaps[POWER_SEQ_GAP_NUM])
{
	memcpy(gaps, seq_gaps_us, sizeof(seq_gaps_us));
}

void power_seq_init(void)
{
	int i;

	power_is_on = 0;
	for (i = 0; i < POWER_SEQ_GAP_NUM; ++i)
		seq_gaps_us[i] = POWER_SEQ_DEFAULT_GAP_US;
	seq_gaps_us[0] = POWER_SEQ_FIRST_GAP_US;	/* EN12V->PWREN1: 80ms */
}

void power_seq_on(void)
{
	int i;

	debug("power_seq: ON sequence start\n");

	for (i = 0; i < POWER_SEQ_NUM; ++i) {
		if (i > 0)
			timer_delay_us(seq_gaps_us[i - 1]);
		debug("  %-12s -> HIGH\n", power_seq_nodes[i].name);
		gpio_bit_set(power_seq_nodes[i].port, power_seq_nodes[i].pin);
	}

	power_is_on = 1;
	debug("power_seq: ON sequence done\n");
}

void power_seq_off(void)
{
	int i;

	debug("power_seq: OFF sequence start\n");

	for (i = POWER_SEQ_NUM - 1; i >= 0; --i) {
		debug("  %-12s -> LOW\n", power_seq_nodes[i].name);
		gpio_bit_reset(power_seq_nodes[i].port, power_seq_nodes[i].pin);
	}

	power_is_on = 0;
	debug("power_seq: OFF sequence done\n");
}

int power_seq_status(void)
{
	return power_is_on;
}
