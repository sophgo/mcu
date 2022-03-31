#include <libopencm3/cm3/nvic.h>
#include <libopencmsis/core_cm3.h>
#include <pin.h>
#include <pic.h>
#include <tca6416a.h>
#include <power.h>
#include <keyboard.h>
#include <tick.h>
#include <loop.h>
#include <se6.h>
#include <chip.h>
#include <wdt.h>
#include <mcu.h>
#include <at24c128c-e2prom.h>

#define DDR_TYPE	DDR_TYPE_LPDDR4X
#define KEY_RD_TIME	(50)

/* virtual key define */
#define VIRT_POWER_KEY_PORT	0
#define VIRT_POWER_KEY		(1 << 0)

#define REBOOT_KEY_PORT		0
#define REBOOT_KEY		(1 << 1)

#define FACTORY_RESET_KEY_PORT	0
#define FACTORY_RESET_KEY	(1 << 2)

#define POWER_KEY_TIMER		(2 * 1000)
#define FACTRST_KEY_TIMER	(10 * 1000)

/*redefine key in se6*/
#define SE6_PWR_KEY_PORT	MCU_INT_PORT
#define SE6_PWR_KEY_PIN		MCU_INT_PIN

#define SE6_PWR_ON_PORT		MCU_UART1_RX_PORT
#define SE6_PWR_ON_PIN		MCU_UART1_RX_PIN

enum {
	KEY_NONE,
	KEY_POWER,
	KEY_FACREST,
};

static struct pwower_key{
	uint16_t press_cnt;
	uint8_t key_value;
	bool	key_pressed:1;
	bool	key_need_release:1;
} se6_pwerkey;

static inline void key_spin_lock(void)
{
	__disable_irq();
}

static inline void key_spin_unlock(void)
{
	__enable_irq();
}

static uint16_t se6ctrl_get_pwrkey(void)
{
	return gpio_get(SE6_PWR_KEY_PORT, SE6_PWR_KEY_PIN);
}

static void se6ctrl_set_recovery(void)
{
	mcu_set_gp0(MCU_CMD_RECOVERY);
	kbd_set(FACTORY_RESET_KEY_PORT, FACTORY_RESET_KEY);
	chip_reset();
	wdt_reset();
}

static void se6ctrl_set_keyvalue(uint16_t key)
{
	key_spin_lock();
	se6_pwerkey.key_value = key;
	key_spin_unlock();

}

static void se6ctrl_key_detect(void)
{
	if (!se6_pwerkey.key_need_release && (se6ctrl_get_pwrkey() != 0)) {
		se6_pwerkey.key_pressed = true;
		se6_pwerkey.press_cnt++;
	} else if (se6ctrl_get_pwrkey() == 0) {
		se6_pwerkey.key_pressed = false;
		se6_pwerkey.key_need_release = false;
	}

	/* when release judge key value */
	if (!se6_pwerkey.key_pressed) {
		if (se6_pwerkey.press_cnt * KEY_RD_TIME >= POWER_KEY_TIMER)
			se6ctrl_set_keyvalue(KEY_POWER);
		se6_pwerkey.press_cnt = 0;
	} else {
	/* when pressed judge key value */
		if (se6_pwerkey.press_cnt * KEY_RD_TIME >= FACTRST_KEY_TIMER) {
			se6ctrl_set_keyvalue(KEY_FACREST);
			se6_pwerkey.key_pressed = false;
			se6_pwerkey.key_need_release = true;
			se6_pwerkey.press_cnt = 0;
		}
	}

}

static void se6ctrl_set_pwron(bool poweron)
{
	if (poweron)
		gpio_set(SE6_PWR_ON_PORT, SE6_PWR_ON_PIN);
	else
		gpio_clear(SE6_PWR_ON_PORT, SE6_PWR_ON_PIN);
}

static void se6ctrl_pwroff_do(void)
{
	kbd_set(VIRT_POWER_KEY_PORT, VIRT_POWER_KEY);
	mdelay(50);
	se6ctrl_set_pwron(false);
}

static void se6ctrl_pwrkey_detect(void)
{
	tick_register_task(se6ctrl_key_detect, KEY_RD_TIME);
}

static void se6_process(void)
{
	/* handle the key action */
	if (se6_pwerkey.key_value == KEY_FACREST) {
		se6ctrl_set_recovery();
		se6ctrl_set_keyvalue(KEY_NONE);
	} else if (se6_pwerkey.key_value == KEY_POWER) {
		se6ctrl_pwroff_do();
		se6ctrl_set_keyvalue(KEY_NONE);
	}
}

static void se6ctrl_init_board(void)
{
	gpio_clear(SE6_PWR_KEY_PORT, SE6_PWR_KEY_PIN);
	gpio_mode_setup(SE6_PWR_KEY_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
			SE6_PWR_KEY_PIN);

	gpio_clear(SE6_PWR_ON_PORT, SE6_PWR_ON_PIN);
	gpio_mode_setup(SE6_PWR_ON_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN,
			SE6_PWR_ON_PIN);

	se6ctrl_set_pwron(true);
}

void se6_init(void)
{
	se6ctrl_init_board();
	se6ctrl_pwrkey_detect();
	loop_add(se6_process);
}
