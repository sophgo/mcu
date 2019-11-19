#include <i2c_slave.h>
#include <upgrader.h>
#include <stm32l0xx_hal.h>
#include <checksum.h>

#define MCU_LED_PIN		GPIO_PIN_5

struct i2c_slave_ctx i2c1;
struct i2c_slave_ctx i2c3;
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

volatile uint8_t app_checksum[16];

int main(void)
{
	// asm volatile ("loop: b loop");
	i2c1.id = 1;
	i2c3.id = 3;
	/* enable i2c by write cr1 pe bit to 1, in function i2c_slave_init */
	i2c_slave_init(&i2c1, (void *)I2C1_BASE);
	i2c_slave_init(&i2c3, (void *)I2C3_BASE);
	/* reenable interrupt */
	__enable_irq();
	/* re-enable systick */
	SysTick->CTRL |= 1;

	upgrader_init();
	i2c_slave_start(&i2c1);
	i2c_slave_start(&i2c3);
	while (1) {
		led_update();
		upgrader_calc_cksum();
	}
	return 0;
}

