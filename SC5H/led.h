#ifndef __LED_H__
#define __LED_H__

static inline void led_off(void);

static inline void led_init(void)
{
	gpio_mode_setup(LED_PORT,
			GPIO_MODE_OUTPUT,
			GPIO_PUPD_PULLUP,
			LED_PIN);
	led_off();
}

static inline void led_on(void)
{
	gpio_set(LED_PORT, LED_PIN);
}

static inline void led_off(void)
{
	gpio_clear(LED_PORT, LED_PIN);
}

#endif
