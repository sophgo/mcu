#include <i2c_slave.h>
#include <upgrader.h>
#include <stm32l0xx_hal.h>
#include <checksum.h>

#define MCU_LED_PIN		GPIO_PIN_5

volatile uint32_t tick;

void delay(unsigned int ms)
{
	unsigned int start = tick;
	while (tick - start < ms)
		;
}

static inline void led_on(void)
{
	HAL_GPIO_WritePin(GPIOA, MCU_LED_PIN, GPIO_PIN_SET);
}

static inline void led_off(void)
{
	HAL_GPIO_WritePin(GPIOA, MCU_LED_PIN, GPIO_PIN_RESET);
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

int main(void)
{
	/* reenable interrupt */
	__enable_irq();
	/* re-enable systick */
	SysTick->CTRL |= 1;

	upgrader_init();
	while (1) {
		led_update();
		upgrader_calc_cksum();
	}
	return 0;
}

