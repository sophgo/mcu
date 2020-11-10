#include <stdint.h>
#include <project.h>
#include <libopencm3/stm32/gpio.h>
#include <common.h>
#include <pin.h>

#define MCU_SW_VER	0
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
