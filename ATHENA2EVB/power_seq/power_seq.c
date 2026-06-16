/*
 * power_seq.c: CV84 wishlist 上电时序 (ATHENA2 引脚映射)
 *
 * -u: 相邻引脚之间等待 gap(us) 后依次拉高
 * -d: 6 个引脚立即全部拉低（无间隔）
 */
#include <gd32e50x_gpio.h>
#include <timer/timer.h>
#include <gpio_ctrl/gpio_ctrl.h>
#include <power_seq/power_seq.h>
#include <debug.h>
#include <string.h>

static int power_is_on;
static unsigned int seq_gaps_us[POWER_SEQ_GAP_NUM];

static struct gpio_test_node *power_seq_nodes[POWER_SEQ_NUM] = {
	&en_12v0, &poweren1, &poweren2, &poweren3,
	&sys_rstn_h, &pwr_rstn_h,
};

static void seq_set_high(int idx)
{
	gpio_ctrl_set(power_seq_nodes[idx]);
}

static void seq_set_low(int idx)
{
	gpio_ctrl_clear(power_seq_nodes[idx]);
}

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
}

void power_seq_on(void)
{
	int i;

	for (i = 0; i < POWER_SEQ_NUM; ++i) {
		if (i > 0)
			timer_delay_us(seq_gaps_us[i - 1]);
		debug("  %-12s -> HIGH\n", power_seq_nodes[i]->name);
		seq_set_high(i);
	}

	power_is_on = 1;
}

void power_seq_off(void)
{
	int i;

	for (i = POWER_SEQ_NUM - 1; i >= 0; --i) {
		debug("  %-12s -> LOW\n", power_seq_nodes[i]->name);
		seq_set_low(i);
	}

	power_is_on = 0;
}

int power_seq_status(void)
{
	return power_is_on;
}
