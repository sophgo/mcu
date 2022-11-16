#include <stdint.h>
#include <common.h>
#include <pin.h>
#include <tick.h>
#include <timer.h>

#define MCU_SW_VER	0

static uint8_t device_type;
static uint8_t work_mode;

uint8_t get_firmware_version(void)
{
	return MCU_SW_VER;
}

void set_device_type(uint8_t type)
{
	device_type = type;
}

uint8_t get_device_type(void)
{
	return device_type;
}

void device_init(void)
{
	/* donot probe twice */
	if (work_mode)
		return;
	work_mode = 1;
}

int get_work_mode(void)
{
	return work_mode;
}

void led_1_0_on(void)
{
	gpio_bit_set(LED_1_0_PORT, LED_1_0_PIN);
	gpio_bit_reset(LED_1_1_PORT, LED_1_1_PIN);
}

void led_1_1_on(void)
{
	gpio_bit_set(LED_1_1_PORT, LED_1_1_PIN);
	gpio_bit_reset(LED_1_0_PORT, LED_1_0_PIN);
}

void led_2_0_on(void)
{
	gpio_bit_set(LED_2_0_PORT, LED_2_0_PIN);
	gpio_bit_reset(LED_2_1_PORT, LED_2_1_PIN);
}

void led_2_1_on(void)
{
	gpio_bit_set(LED_2_1_PORT, LED_2_1_PIN);
	gpio_bit_reset(LED_2_0_PORT, LED_2_0_PIN);
}
void led_3_0_on(void)
{
	gpio_bit_set(LED_3_0_PORT, LED_3_0_PIN);
	gpio_bit_reset(LED_3_1_PORT, LED_3_1_PIN);
}

void led_3_1_on(void)
{
	gpio_bit_set(LED_3_1_PORT,LED_3_1_PIN);
	gpio_bit_reset(LED_3_0_PORT, LED_3_0_PIN);
}
void led_4_0_on(void)
{
	gpio_bit_set(LED_4_0_PORT, LED_4_0_PIN);
	gpio_bit_reset(LED_4_1_PORT,LED_4_1_PIN);
}

void led_4_1_on(void)
{
	gpio_bit_set(LED_4_1_PORT,LED_4_1_PIN);
	gpio_bit_reset(LED_4_0_PORT, LED_4_0_PIN);
}

void led_flash(int times)
{
	int time;
	for (time = 0;time < times; time++){
		/* delay 250 ms */
		timer_mdelay(250);

		/* turn on LED1 and turn off LED2,LED3,LED4 */
		led_1_0_on();
		led_2_1_on();
		led_3_1_on();
		led_4_1_on();
		/* delay 250 ms */
		timer_mdelay(250);

		/* turn on LED2 and turn off LED1,LED3,LED4 */
		led_1_1_on();
		led_2_0_on();
		led_3_1_on();
		led_4_1_on();

		/* delay 250 ms */
		timer_mdelay(250);

		/* turn on LED3 and turn off LED1,LED2,LED4 */
		led_1_1_on();
		led_2_1_on();
		led_3_0_on();
		led_4_1_on();

		/* delay 250 ms */
		timer_mdelay(250);

		/* turn on LED4 and turn off LED1,LED2,LED3 */
		led_1_1_on();
		led_2_1_on();
		led_3_1_on();
		led_4_0_on();

		/* delay 250 ms */
		timer_mdelay(250);

		/* turn off all */
		led_1_1_on();
		led_2_1_on();
		led_3_1_on();
		led_4_1_on();
	}
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
		led_1_0_on();
	} else if (freq == 0) {
		tick_set_task_interval(led_tick_handle, 0);
		led_1_1_on();
	} else {
		tick_set_task_interval(led_tick_handle, 1000 / freq / 2);
	}
}

void led_isr(void)
{
	led_status = !led_status;
	if (led_status)
		led_1_0_on();
	else
		led_1_1_on();
}
