#include <stdint.h>
#include <project.h>
#include <common.h>
#include <pin.h>
#include <tick.h>
#include <adc.h>

#define DDR_TYPE	DDR_TYPE_LPDDR4X
#define LED_MAX		4

#define LED1_PORT	STATUS_LED0_PORT
#define LED1_PIN	STATUS_LED0_PIN

static uint8_t board_type;

static int board_temp, soc_temp;
static struct
{
	int num, port, pin;
}leds[LED_MAX] = {
	{1, LED1_PORT, LED1_PIN},
};


int get_board_temp(void)
{
	return board_temp;
}

int get_soc_temp(void)
{
	return soc_temp;
}

void set_board_temp(int temp)
{
	board_temp = temp;
}

void set_soc_temp(int temp)
{
	soc_temp = temp;
}

uint8_t get_ddr_type(void)
{
	return DDR_TYPE;
}

uint8_t get_firmware_version(void)
{
	return MCU_SW_VER;
}

char *get_board_type_name()
{
	switch (board_type) {
	case BM1690EVB:
		return "BM1690 EVB";

	};
	/* U means unknown type */
	return "bm1690-U";
}

static void set_board_type(uint8_t type)
{
	board_type = type;
}

uint8_t get_board_type(void)
{
	return board_type;
}

void board_init(void)
{
	set_board_type(BM1690EVB);
}

void led_control(int n, int status)
{
	if (status == ON) {
		for (int i = 0; i < LED_MAX; i++) {
			if (leds[i].num == n)
				gpio_set(leds[i].port, leds[i].pin);
		}
	} else if (status == OFF) {
		for (int i = 0; i < LED_MAX; i++) {
			if (leds[i].num == n)
				gpio_clear(leds[i].port, leds[i].pin);
		}
	}
}

void led_on(void)
{
	gpio_bit_set(STATUS_LED0_PORT, STATUS_LED0_PIN);
}

void led_off(void)
{
	gpio_bit_reset(STATUS_LED0_PORT, STATUS_LED0_PIN);
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
