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

/*second*/
#define SE6_POWER_OFF_DELAY		(15)
#define KEY_RD_TIME				(50)

/* virtual key define */
#define VIRT_POWER_KEY_PORT		(0)
#define VIRT_POWER_KEY			(1 << 0)

#define REBOOT_KEY_PORT			(0)
#define REBOOT_KEY				(1 << 1)

#define VIRT_RESTART_KEY_PORT			(0)
#define VIRT_RESTART_KEY				(1 << 2)

#define POWER_KEY_TIMER			(2 * 1000)
#define SLEEP_ON_KEY_TIMER		(3 * 1000)
#define SLEEP_OFF_KEY_TIMER		(1 * 1000)
#define FACTRST_KEY_TIMER		(30 * 1000)
#define RESTART_KEY_TIMER		(4 * 1000)

/*redefine key in se6*/
#define SE6_PWR_KEY_PORT	PMEB_PORT
#define SE6_PWR_KEY_PIN		PMEB_PIN

#define SE6_PWR_ON_PORT		MCU_UART1_RX_PORT
#define SE6_PWR_ON_PIN		MCU_UART1_RX_PIN

#define POWROFF_TIMER_NO_CUSTOMIZED   (0xffff)
enum {
	KEY_NONE,
	KEY_POWER,
	KEY_SLEEP_ON,
	KEY_SLEEP_OFF,
	KEY_RESTART,
	KEY_FACREST,
};

static struct power_off{
	uint32_t count;
	bool	 start:1;
	uint16_t timer;
} se6_restart;

static struct power_key{
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

static int se6ctrl_get_pwrkey_value(void)
{
	if (!gpio_get(SE6_PWR_KEY_PORT, SE6_PWR_KEY_PIN))
		return 0;
	return 1;
}

static int se6ctrl_is_pwrkey_pressed(void)
{
	static int key_release_value;

	key_release_value = (get_eeprom_type() == AT24C128C ? 0 : 1);
	if (se6ctrl_get_pwrkey_value() == key_release_value)
		return 0;
	return 1;
}

static void se6ctrl_set_keyvalue(uint16_t key)
{
	key_spin_lock();
	se6_pwerkey.key_value = key;
	key_spin_unlock();

}

static void se6ctrl_key_detect(void)
{
	if ((!se6_pwerkey.key_need_release) && se6ctrl_is_pwrkey_pressed()) {
		se6_pwerkey.key_pressed = true;
		se6_pwerkey.press_cnt++;
	} else if (!se6ctrl_is_pwrkey_pressed()) {
		se6_pwerkey.key_pressed = false;
		se6_pwerkey.key_need_release = false;
	}

	/* when release judge key value */
	if (!se6_pwerkey.key_pressed) {
		se6_pwerkey.press_cnt = 0;
	} else {
	/* when pressed judge key value */
		if (se6_pwerkey.press_cnt * KEY_RD_TIME >= RESTART_KEY_TIMER) {
			se6ctrl_set_keyvalue(KEY_RESTART);
			se6_pwerkey.key_pressed = false;
			se6_pwerkey.key_need_release = true;
			se6_pwerkey.press_cnt = 0;
		}
	}

}

void se6ctrl_clean_restart(void)
{
	se6_restart.start = false;
	se6_restart.count = 0;
}

void se6ctrl_(void)
{
//	chip_reset();
	chip_popd_reset_early();
	chip_popd_reset_end();
}

static void se6_wait_restart(void)
{
	if (se6_restart.start == false)
		return;
	if (tick_get() - se6_restart.count > se6_restart.timer * 1000){
		se6ctrl_();
		se6ctrl_clean_restart();
	}
}

void se6ctrl_set_pwron(bool poweron)
{
	if (poweron)
		gpio_set(SE6_PWR_ON_PORT, SE6_PWR_ON_PIN);
	else
		gpio_clear(SE6_PWR_ON_PORT, SE6_PWR_ON_PIN);
}

static void se6ctrl_restart_do(void)
{
	if (se6_restart.start == true)
		return;
	kbd_set(REBOOT_KEY_PORT, REBOOT_KEY);
	se6_restart.count = tick_get();
	se6_restart.start = true;
}

static void se6ctrl_pwrkey_detect(void)
{
	tick_register_task(se6ctrl_key_detect, KEY_RD_TIME);
}

static void se6_process(void)
{
	/* handle the key action */
	if (se6_pwerkey.key_value == KEY_RESTART) {
		se6ctrl_restart_do();
		se6ctrl_set_keyvalue(KEY_NONE);
	}

	se6_wait_restart();
}

static void se6ctrl_init_board(void)
{
	uint16_t pwr_timer = 0;

	gpio_clear(SE6_PWR_KEY_PORT, SE6_PWR_KEY_PIN);
	gpio_mode_setup(SE6_PWR_KEY_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
			SE6_PWR_KEY_PIN);

	gpio_clear(SE6_PWR_ON_PORT, SE6_PWR_ON_PIN);
	gpio_mode_setup(SE6_PWR_ON_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN,
			SE6_PWR_ON_PIN);

	se6ctrl_set_pwron(true);

	pwr_timer = at24_get_pwroff_timer();
	if (pwr_timer == POWROFF_TIMER_NO_CUSTOMIZED)
		se6_restart.timer = SE6_POWER_OFF_DELAY;
	else
		se6_restart.timer = pwr_timer;
}

void se6_init(void)
{
	se6ctrl_init_board();
	se6ctrl_pwrkey_detect();
	loop_add(se6_process);
}
