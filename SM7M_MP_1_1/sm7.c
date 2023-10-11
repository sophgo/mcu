#include <i2c_slave.h>
#include <common.h>
#include <tca6416a.h>
#include <tick.h>
#include <loop.h>

/* count in ms */
#define SM7_POWER_OFF_DELAY	5000

/* gpio expandtion(tca6416a) config */
/* port 0 */

#define POWER_ENABLE_PORT		0
#define POWER_ENABLE_PIN		0
#define POWER_ENABLE_DEFAULT	1

#define LED0_PORT		0
#define LED0_PIN		3
#define LED0_DEFAULT		1

#define LED1_PORT		0
#define LED1_PIN		4
#define LED1_DEFAULT		0

/* input */
#define POWER_BUTTON_PORT	0
#define POWER_BUTTON_PIN	5

#define P0_CFG		(1 << POWER_BUTTON_PIN)
#define P0_OUT		((POWER_ENABLE_DEFAULT << POWER_ENABLE_PIN) | \
			 (LED0_DEFAULT << LED0_PIN) | \
			 (LED1_DEFAULT << LED1_PIN))
#define P0_POL		0


/* port 1 */
#define LTE_ENABLE_PORT		1
#define LTE_ENABLE_PIN		0
#define LTE_ENABLE_DEFAULT	1

/* input */
#define PCIE_INT_PORT		1
#define PCIE_INT_PIN		1

#define SATA_ENABLE_PORT	1
#define SATA_ENABLE_PIN		2
#define SATA_ENABLE_DEFAULT	1

#define P1_CFG		(1 << PCIE_INT_PIN)
#define P1_OUT		((LTE_ENABLE_DEFAULT << LTE_ENABLE_PIN) | \
			 (SATA_ENABLE_DEFAULT << SATA_ENABLE_PIN))
#define P1_POL		0

static const struct {
	uint8_t port, pin;
} sm7_leds[] = {
	{LED0_PORT, LED0_PIN},
	{LED1_PORT, LED1_PIN}
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

static uint32_t sm7_timer_start;

static void sm7_process(void)
{
	int err;

	err = tca6416a_get(POWER_BUTTON_PORT, POWER_BUTTON_PIN);
	if (err < 0)
		return;

	if (err) {
		if (sm7_timer_start == 0)
			sm7_timer_start = tick_get();
		else {
			if (tick_get() - sm7_timer_start >
			    SM7_POWER_OFF_DELAY) {
				sm7_led_off(0);
				sm7_12v_off();
			}
		}
	} else {
		sm7_timer_start = 0;
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

	loop_add(sm7_process);
}
