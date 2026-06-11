#include <gd32f4xx_gpio.h>
#include <stdint.h>
#include <project.h>
#include <common.h>
#include <pin.h>
#include <tick.h>
#include <debug.h>
#include <system.h>

#define DDR_TYPE	DDR_TYPE_LPDDR4X

static uint8_t location;
static uint8_t board_type = RHS12;

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

uint8_t get_firmware_version(void)
{
	return MCU_SW_VER;
}

void set_module_id(void)
{
	int i;
	int pin_count = 3;

	/* RHS12 uses 4 module ID pins, HD12 uses 3 */
	if (get_board_type() == RHS12)
		pin_count = 4;

	for (i = 0; i < pin_count; i++) {
		uint32_t pin;
		switch (i) {
		case 0: pin = MODULE_ID_0_PIN; break;
		case 1: pin = MODULE_ID_1_PIN; break;
		case 2: pin = MODULE_ID_2_PIN; break;
		case 3: pin = MODULE_ID_3_PIN; break;
		default: pin = 0; break;
		}
		if (gpio_input_bit_get(GPIOC, pin) == 1)
			location |= 1 << i;
	}
}

uint8_t get_module_id(void)
{
	return location;
}

void board_type_init(void)
{
	/* MODULE_PG (PA12) determines board type:
	 *   PG = 0 -> HD12
	 *   PG = 1 -> RHS12
	 */
	if (gpio_input_bit_get(MODULE_PG_PORT, MODULE_PG_PIN))
		board_type = RHS12;
	else
		board_type = HD12;
	
    dbg_printf("%s, board_type = 0x%08x\n", __func__, board_type);
}

uint8_t get_board_type(void)
{
	return board_type;
}
