#include <stdint.h>
#include <project.h>
#include <libopencm3/stm32/gpio.h>
#include <common.h>
#include <pin.h>
#include <tick.h>

#define MCU_SW_VER	17
#define DDR_TYPE	DDR_TYPE_LPDDR4X

static uint8_t board_type;
static uint8_t work_mode;

uint8_t get_ddr_type(void)
{
	return DDR_TYPE;
}

char *get_board_type_name()
{
	switch (board_type) {
	case SM5ME:
		return "SM5-MINI-E";
	case SM5MS:
		return "SM5-MINI-S";
	case SM5MP:
		return "SM5-MINI-P";
	case SM5MA:
		return "SM5-MINI-A";
	case SM5SE6M:
		return "SE6-CTRL";

	};
	/* U means unknown type */
	return "SM5-MINI-U";
}

void set_board_type(uint8_t type)
{
	board_type = type;
}

uint8_t get_board_type(void)
{
	return board_type;
}

uint8_t get_firmware_version(void)
{
	return MCU_SW_VER;
}

void board_init(void)
{
	/* donot probe twice */
	if (work_mode)
		return;

	work_mode = gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN) ?
		WORK_MODE_SOC : WORK_MODE_PCIE;
}

int get_work_mode(void)
{
	return work_mode;
}

uint8_t get_declared_board_type(void)
{
	return *(uint8_t*)EEPROM_BOARD_TYPE_ADDR;
}

static inline void led_on(void)
{
	gpio_set(MCU_LED_PORT, MCU_LED_PIN);
}

static inline void led_off(void)
{
	gpio_clear(MCU_LED_PORT, MCU_LED_PIN);
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

int detect_test_mode(void)
{
	return	(!!gpio_get(MCU_UART1_TX_PORT, MCU_UART1_TX_PIN) << 0) |
		(!!gpio_get(MCU_UART1_RX_PORT, MCU_UART1_RX_PIN) << 1);
}
