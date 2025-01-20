#ifndef __RST_KEY___
#define __RST_KEY__

#define RST_KEY_NVIC	NVIC_EXTI4_15_IRQ

void rst_key_init(void);
void rst_key_isr(void);

#endif