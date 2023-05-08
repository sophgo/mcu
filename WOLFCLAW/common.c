#include <stdint.h>
#include <common.h>
#include <pin.h>
#include <tick.h>
#include <timer.h>

static uint8_t default_device;
static uint8_t default_addr;
static char *default_filename;

void set_default_device(uint8_t type)
{
	default_device = type;
}

uint8_t get_default_device(void)
{
	return default_device;
}

void set_default_addr(uint8_t addr)
{
	default_addr = addr;
}

uint8_t get_default_addr(void)
{
	return default_addr;
}

void set_default_file_name(char* name)
{
	default_filename = name;
}

char *get_default_filename()
{
	return default_filename;
}

FlagStatus is_key_pressed()
{
	if (gpio_get(KEY_PORT, KEY_PIN) == RESET)
		return SET;
	else
		return RESET;
}

FlagStatus is_sdcard_inserted()
{
	if (gpio_get(SDIO_CD_X_PORT, SDIO_CD_X_PIN) == RESET)
		return SET;
	else
		return RESET;
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
