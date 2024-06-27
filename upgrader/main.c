#include <i2c_slave.h>
#include <upgrader.h>
#include <stm32l0xx_hal.h>
#include <checksum.h>
#include <flash.h>
#include <project.h>

static unsigned int MCU_LED_PIN;
static GPIO_TypeDef *MCU_LED_PORT;
extern uint32_t project;

volatile uint32_t tick;

void delay(unsigned int ms)
{
	unsigned int start = tick;
	while (tick - start < ms)
		;
}

static inline void led_on(void)
{
	if (MCU_LED_PORT)
		HAL_GPIO_WritePin(MCU_LED_PORT, MCU_LED_PIN, GPIO_PIN_SET);
}

static inline void led_off(void)
{
	if (MCU_LED_PORT)
		HAL_GPIO_WritePin(MCU_LED_PORT, MCU_LED_PIN, GPIO_PIN_RESET);
}

static inline void led_update(void)
{
	/* flicker at about 2Hz */
	// tick = (tick / 256) & 1;
	// tick &= 1 << 7; //faster then divide

	if (tick & (1 << 7))
		led_on();
	else
		led_off();
}

static inline void led_init(void)
{
	switch (project) {
		case SA5:
		case SM5P:
		case SM5S:
			MCU_LED_PORT = GPIOA;
			MCU_LED_PIN = GPIO_PIN_5;
			break;
		case SM5ME:
		case SM5MP:
		case SM5MS:
		case SM5MA:
			MCU_LED_PORT = GPIOC;
			MCU_LED_PIN = GPIO_PIN_14;
			break;
		case BM1684XEVB:
		case SM7M:
		case SM7MQY:
		case SE7Q:
		case SM7M_MP_1_1:
		case SM7_HK:
		case SE7:
		case SM7MSE6M:
		case SM7CUSTV1:
			MCU_LED_PORT = GPIOC;
			MCU_LED_PIN = GPIO_PIN_13;
			break;
		default:
			MCU_LED_PORT = NULL;
			break;
	}
}
extern int puts(const char *s);
int main(void)
{
	flash_init();
	/* reenable interrupt */
	__enable_irq();
	/* re-enable systick */
	SysTick->CTRL |= 1;

	led_init();
	upgrader_init();
	while (1) {
		led_update();
		upgrader_calc_cksum();
	}
	return 0;
}

