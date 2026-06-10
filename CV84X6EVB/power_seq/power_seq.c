/*
 * power_seq.c: CV84x6 上电时序控制
 *
 * 上电顺序: EN_12V0 -> POWEREN1 -> POWEREN2 -> POWEREN3 -> SYS_RSTN_H -> PWR_RSTN_H
 * 下电顺序: 反向
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_gpio.h>
#include <timer/timer.h>
#include <pin.h>
#include <power_seq/power_seq.h>
#include <debug.h>

static int power_is_on;
static unsigned int seq_delay_ms = 2;

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

void power_seq_on_with_delays(const unsigned int delays[POWER_SEQ_NUM - 1])
{
	int i;

	if (power_is_on) {
		debug("power_seq: already ON\n");
		return;
	}

	debug("power_seq: ON sequence start (custom delays)\n");

	for (i = 0; i < POWER_SEQ_NUM; ++i) {
		debug("  %-12s -> HIGH\n", power_seq_nodes[i].name);
		gpio_bit_set(power_seq_nodes[i].port, power_seq_nodes[i].pin);
		if (i < POWER_SEQ_NUM - 1)
			timer_delay_ms(delays[i]);
	}

	power_is_on = 1;
	debug("power_seq: ON sequence done\n");
}

void power_seq_init(void)
{
	power_is_on = 0;
}

void power_seq_on(void)
{
	int i;

	if (power_is_on) {
		debug("power_seq: already ON\n");
		return;
	}

	debug("power_seq: ON sequence start, delay=%ums\n", seq_delay_ms);

	for (i = 0; i < POWER_SEQ_NUM; ++i) {
		debug("  %-12s -> HIGH\n", power_seq_nodes[i].name);
		gpio_bit_set(power_seq_nodes[i].port, power_seq_nodes[i].pin);
		timer_delay_ms(seq_delay_ms);
	}

	power_is_on = 1;
	debug("power_seq: ON sequence done\n");
}

void power_seq_off(void)
{
	int i;

	if (!power_is_on) {
		debug("power_seq: already OFF\n");
		return;
	}

	debug("power_seq: OFF sequence start\n");

	for (i = POWER_SEQ_NUM - 1; i >= 0; --i) {
		debug("  %-12s -> LOW\n", power_seq_nodes[i].name);
		gpio_bit_reset(power_seq_nodes[i].port, power_seq_nodes[i].pin);
		timer_delay_ms(seq_delay_ms);
	}

	power_is_on = 0;
	debug("power_seq: OFF sequence done\n");
}

void power_seq_set_delay_ms(unsigned int ms)
{
	seq_delay_ms = ms;
}

unsigned int power_seq_get_delay_ms(void)
{
	return seq_delay_ms;
}

int power_seq_status(void)
{
	return power_is_on;
}