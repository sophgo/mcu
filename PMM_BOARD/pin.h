/*
 * pin.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __PIN_H__
#define __PIN_H__

#include <gd32e50x_gpio.h>

/* port A definition */
#define CH4_I_PORT			GPIOA
#define CH4_I_PIN			GPIO_PIN_0
#define CH4_I_EXTI			EXTI_0
#define CH3_I_PORT			GPIOA
#define CH3_I_PIN			GPIO_PIN_1
#define CH3_I_EXTI			EXTI_1
#define CH2_I_PORT			GPIOA
#define CH2_I_PIN			GPIO_PIN_2
#define CH2_I_EXTI			EXTI_2
#define NTC2_PORT			GPIOA
#define NTC2_PIN			GPIO_PIN_3
#define NTC2_EXTI			EXTI_3
#define CH13_I_PORT			GPIOA
#define CH13_I_PIN			GPIO_PIN_4
#define CH13_I_EXTI			EXTI_4
#define CH8_I_PORT			GPIOA
#define CH8_I_PIN			GPIO_PIN_5
#define CH8_I_EXTI			EXTI_5
#define CH14_I_PORT			GPIOA
#define CH14_I_PIN			GPIO_PIN_6
#define CH14_I_EXTI			EXTI_6
#define CH7_I_PORT			GPIOA
#define CH7_I_PIN			GPIO_PIN_7
#define CH7_I_EXTI			EXTI_7
#define MCU_USBDM_PORT			GPIOB
#define MCU_USBDM_PIN			GPIO_PIN_7
#define MCU_USBDM_EXTI			EXTI_7
#define MCU_USBDP_PORT			GPIOB
#define MCU_USBDP_PIN			GPIO_PIN_13
#define MCU_USBDP_EXTI			EXTI_13

/* port B definition */
#define CH16_I_PORT			GPIOB
#define CH16_I_PIN			GPIO_PIN_0
#define CH16_I_EXTI			EXTI_0
#define CH5_I_PORT			GPIOB
#define CH5_I_PIN			GPIO_PIN_1
#define CH5_I_EXTI			EXTI_1
#define MCU_SPI2_SCK_PORT		GPIOB
#define MCU_SPI2_SCK_PIN		GPIO_PIN_3
#define MCU_SPI2_SCK_EXTI		EXTI_3
#define MCU_SPI2_MISO_PORT		GPIOB
#define MCU_SPI2_MISO_PIN		GPIO_PIN_4
#define MCU_SPI2_MISO_EXTI		EXTI_4
#define MCU_SPI2_MOSI_PORT		GPIOD
#define MCU_SPI2_MOSI_PIN		GPIO_PIN_5
#define MCU_SPI2_MOSI_EXTI		EXTI_5
#define MCU_SPI2_SC_HASH_PORT		GPIOD
#define MCU_SPI2_SC_HASH_PIN		GPIO_PIN_6
#define MCU_SPI2_SC_HASH_EXTI		EXTI_6
#define MCU_IIC0_SCL_PORT		GPIOD
#define MCU_IIC0_SCL_PIN		GPIO_PIN_8
#define MCU_IIC0_SCL_EXTI		EXTI_8
#define MCU_IIC0_SDA_PORT		GPIOD
#define MCU_IIC0_SDA_PIN		GPIO_PIN_9
#define MCU_IIC0_SDA_EXTI		EXTI_9

/* port C definition */
#define CH9_I_PORT			GPIOC
#define CH9_I_PIN			GPIO_PIN_0
#define CH9_I_EXTI			EXTI_0
#define CH10_I_PORT			GPIOC
#define CH10_I_PIN			GPIO_PIN_1
#define CH10_I_EXTI			EXTI_1
#define CH11_I_PORT			GPIOC
#define CH11_I_PIN			GPIO_PIN_2
#define CH11_I_EXTI			EXTI_2
#define CH12_I_PORT			GPIOC
#define CH12_I_PIN			GPIO_PIN_3
#define CH12_I_EXTI			EXTI_3
#define CH15_I_PORT			GPIOC
#define CH15_I_PIN			GPIO_PIN_4
#define CH15_I_EXTI			EXTI_4
#define CH6_I_PORT			GPIOC
#define CH6_I_PIN			GPIO_PIN_5
#define CH6_I_EXTI			EXTI_5

/* port D definition */
#define MCU_USART_TX_PORT		GPIOD
#define MCU_USART_TX_PIN		GPIO_PIN_5
#define MCU_USART_TX_EXTI		EXTI_5
#define MCU_USART_RX_PORT		GPIOD
#define MCU_USART_RX_PIN		GPIO_PIN_6
#define MCU_USART_RX_EXTI		EXTI_6

/* port E definition */

#endif /* __PIN_H__ */
