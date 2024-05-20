#include <stdint.h>
#include <project.h>
#include <common.h>
#include <pin.h>
#include <tick.h>

#define DDR_TYPE	DDR_TYPE_LPDDR4X

static uint8_t board_type;
static uint8_t work_mode;

static int board_temp, soc_temp;

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
	case BM2044REVB:
		return "BM2042R EVB";
	};
	/* U means unknown type */
	return "BM2044R-U";
}

void set_board_type(uint8_t type)
{
	board_type = type;
}

uint8_t get_board_type(void)
{
	return board_type;
}

void board_init(void)
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

static void mcu_status_led_on(void)
{
	gpio_bit_set(MCU_STATUS_LED_PORT, MCU_STATUS_LED_PIN);
}

static void mcu_status_led_off(void)
{
	gpio_bit_reset(MCU_STATUS_LED_PORT, MCU_STATUS_LED_PIN);
}

void power_status_led_on(void)
{
	gpio_bit_set(POWER_LED_PORT, POWER_LED_PIN);
}

void power_status_led_off(void)
{
	gpio_bit_reset(POWER_LED_PORT, POWER_LED_PIN);
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
		mcu_status_led_on();
	} else if (freq == 0) {
		tick_set_task_interval(led_tick_handle, 0);
		mcu_status_led_off();
	} else {
		tick_set_task_interval(led_tick_handle, 1000 / freq / 2);
	}
}

void led_isr(void)
{
	led_status = !led_status;
	if (led_status)
		mcu_status_led_on();
	else
		mcu_status_led_off();
}
