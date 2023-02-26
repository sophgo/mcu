#include <stdint.h>
#include <project.h>
#include <common.h>
#include <pin.h>
#include <tick.h>

#define DDR_TYPE	DDR_TYPE_LPDDR4X

void led_on(void)
{
	// gpio_bit_set(LED_PORT, LED_PIN);
}

void led_off(void)
{
	// gpio_bit_reset(LED_PORT, LED_PIN);
}

int led_tick_handle;
int led_status;

void led_isr(void);

void led_init(void)
{
	/* donnot start now */
	led_tick_handle = tick_register_task(led_isr, 0);
}

/* if freq > 500, turn on led
 * if freq == 0, turn off led
 */
void led_set_frequency(unsigned long freq)
{
	if (freq > 500) {
		/* turn off tick task */
		tick_set_task_interval(led_tick_handle, 0);
		led_on();
	} else if (freq == 0) {
		tick_set_task_interval(led_tick_handle, 0);
		led_off();
	} else {
		tick_set_task_interval(led_tick_handle, 1000 / freq / 2);
	}
}

void led_isr(void)
{
	led_status = !led_status;
	if (led_status)
		led_on();
	else
		led_off();
}
