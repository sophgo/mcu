#include <i2c_slave.h>
#include <common.h>
#include <tca6416a.h>
#include <tick.h>
#include <loop.h>
#include <debug.h>
#include <keyboard.h>
#include <libopencm3/stm32/l0/nvic.h>
#include <libopencm3/stm32/i2c.h>

/*virtual key define*/
#define POWER_KEY_PORT          0
#define POWER_KEY               (1 << 0)

/* count in ms */
#define SM7_POWER_OFF_CMD_DELAY	3000 // 3 seconds to send power off command to kernel
#define SM7_POWER_OFF_DELAY	10000 // 10 seconds to force mcu to power off

/* gpio expandtion(tca6416a) config */
/* port 0 */

#define POWER_ENABLE_PORT		0
#define POWER_ENABLE_PIN		0
#define POWER_ENABLE_DEFAULT	0

#define WIFI_RST_PORT		0
#define WIFI_RST_PIN		1
#define WIFI_RST_DEFAULT	1


#define LED1_PORT			0
#define LED1_PIN			3
#define LED1_DEFAULT		1

#define LED2_PORT			0
#define LED2_PIN			4
#define LED2_DEFAULT		0

#define LTE_RST_PORT		0
#define LTE_RST_PIN			7
#define LTE_RST_DEFAULT		0

/* input */
#define POWER_BUTTON_PORT	0
#define POWER_BUTTON_PIN	5

#define P0_CFG		(1 << POWER_BUTTON_PIN)
#define P0_OUT		((POWER_ENABLE_DEFAULT << POWER_ENABLE_PIN) | \
			 (LED1_DEFAULT << LED1_PIN) | (LED2_DEFAULT << LED2_PIN) | \
			 (WIFI_RST_DEFAULT << WIFI_RST_PIN) | (LTE_RST_DEFAULT << LTE_RST_PIN))
#define P0_POL		0


/* port 1 */
#define LTE_ENABLE_PORT		1
#define LTE_ENABLE_PIN		0
#define LTE_ENABLE_DEFAULT	1

/* input */
#define POWER_STATUS_PORT	1
#define POWER_STATUS_PIN	1

#define P1_CFG		(1 << POWER_STATUS_PIN)
#define P1_OUT		(LTE_ENABLE_DEFAULT << LTE_ENABLE_PIN)
#define P1_POL		0

static const struct {
	uint8_t port, pin;
} sm7_leds[] = {
	{LED1_PORT, LED1_PIN},
	{LED2_PORT, LED2_PIN}
};

int sm7_led_on(int led)
{
	if (led >= ARRAY_SIZE(sm7_leds))
		return -1;
	tca6416a_set(sm7_leds[led].port, sm7_leds[led].pin);
	return 0;
}

int sm7_led_off(int led)
{
	if (led >= ARRAY_SIZE(sm7_leds))
		return -1;
	tca6416a_clr(sm7_leds[led].port, sm7_leds[led].pin);
	return 0;
}

int sm7_12v_on()
{
	tca6416a_set(POWER_ENABLE_PORT, POWER_ENABLE_PIN);
	return 0;
}
int sm7_12v_off()
{
	tca6416a_clr(POWER_ENABLE_PORT, POWER_ENABLE_PIN);
	return 0;
}

static uint32_t sm7_timer_start = 0;
static uint8_t sm7_power_on_flag = 0; // 0 closed 1 starting 2 started 3 closing
static uint8_t sm7_power_on_release = 0;
uint8_t sm7_power_off_flag = 0;

static void sm7_process_power_on(void)
{
	sm7_power_on_flag = 1; //starting
	debug("sm7_12v_on!\n");
	sm7_led_on(1);
	sm7_12v_on();
	i2c_peripheral_enable(I2C1);
}

static void sm7_process_power_off(void)
{
	sm7_power_on_flag = 0;
	debug("sm7_12v_off!\n");
	sm7_led_off(1);
	mdelay(1000);
	sm7_12v_off();
	mdelay(1000);
	debug("sm7_12v_off end!\n");
	//i2c_peripheral_enable(I2C1);
}

static void sm7_process(void)
{
	int err;
	uint32_t press_time;
	static int run_status = 0;
	if (sm7_power_off_flag==1){
		sm7_power_off_flag = 0;
		sm7_process_power_off();
	}

	err = tca6416a_get(POWER_BUTTON_PORT, POWER_BUTTON_PIN);
	if (err < 0)
		return;

	if (err) {
		if (sm7_power_on_release) {
			press_time = tick_get() - sm7_timer_start; 
			if (sm7_power_on_flag == 2) {
				if (press_time > SM7_POWER_OFF_DELAY) {
					run_status = 4;
				}
				else if (press_time > SM7_POWER_OFF_CMD_DELAY) {
					run_status = 1;
				}
				else if (run_status != 4)
					run_status = 0;
			}
			else if (run_status != 4)
				run_status = 0;
		}
		else {
			sm7_timer_start = tick_get();
			sm7_power_on_release = 1;
		}
	}
	else {
		sm7_timer_start = 0;
		sm7_power_on_release = 0;
		if (run_status == 1) {
			run_status = 3;
		}
		else {
			run_status = 0;
		}
		if (sm7_power_on_flag == 1)
			sm7_power_on_flag = 2;
	}

	switch (run_status)
	{
		case 0: 
			/* NO DO */
			break;
		case 1:
			/* wait button release */
			break;
		case 2:
			/* poweron */
			sm7_process_power_on();
			break;
		case 3:
			/* poweroff 3s */
			kbd_set(POWER_KEY_PORT, POWER_KEY);
			break;
		case 4:
			/* poweroff 10s */
			sm7_process_power_off();
			break;
		default:
			/* NO DO */
			break;
	}
}

void sm7_init(void)
{
	tca6416a_write(TCA6416A_P0_OUT, P0_OUT);
	tca6416a_write(TCA6416A_P1_OUT, P1_OUT);
	tca6416a_write(TCA6416A_P0_CFG, P0_CFG);
	tca6416a_write(TCA6416A_P1_CFG, P1_CFG);
	tca6416a_write(TCA6416A_P0_POL, P0_POL);
	tca6416a_write(TCA6416A_P1_POL, P1_POL);
	
	sm7_process_power_on();
	loop_add(sm7_process);
}

