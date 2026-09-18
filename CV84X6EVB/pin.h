/*
 * pin.h:
 *
 * (C) Copyright 2024 Sophgo Technology
 * CV84X6EVB 引脚定义
 */
#ifndef __PIN_H__
#define __PIN_H__

#include <gd32e50x_gpio.h>

/* ================================================================
 * LED
 * ================================================================ */
#define PG_IND_PORT		GPIOC
#define PG_IND_PIN		GPIO_PIN_9

/* ================================================================
 * Power Good 检测 (输入，上拉)
 * ================================================================ */
#define PG_VDD_CORE_PORT	GPIOE
#define PG_VDD_CORE_PIN		GPIO_PIN_2

#define DDR_VDD_PG_PORT		GPIOE
#define DDR_VDD_PG_PIN		GPIO_PIN_3

#define DDR_VDDQ_PG_PORT	GPIOE
#define DDR_VDDQ_PG_PIN		GPIO_PIN_4

#define PG_5V_PORT		GPIOE
#define PG_5V_PIN		GPIO_PIN_5

/* ================================================================
 * GPIO 控制输出
 * ================================================================ */
#define PCIE_SEL_PORT		GPIOA
#define PCIE_SEL_PIN		GPIO_PIN_12

#define EN_12V0_PORT		GPIOA
#define EN_12V0_PIN		GPIO_PIN_11

#define POWEREN1_PORT		GPIOA
#define POWEREN1_PIN		GPIO_PIN_10

#define POWEREN2_PORT		GPIOA
#define POWEREN2_PIN		GPIO_PIN_9

#define POWEREN3_PORT		GPIOA
#define POWEREN3_PIN		GPIO_PIN_8

#define SYS_RSTN_H_PORT		GPIOD
#define SYS_RSTN_H_PIN		GPIO_PIN_10

#define PWR_RSTN_H_PORT		GPIOB
#define PWR_RSTN_H_PIN		GPIO_PIN_13

#define PWR_BUTTON1_H_PORT	GPIOB
#define PWR_BUTTON1_H_PIN	GPIO_PIN_14

#define PWR_ON_H_PORT		GPIOB
#define PWR_ON_H_PIN		GPIO_PIN_15

#define PWR_WAKEUP_H_PORT	GPIOD
#define PWR_WAKEUP_H_PIN	GPIO_PIN_9

/* ================================================================
 * UART2 (控制台)
 * ================================================================ */
#define MCU_UART_TX_PORT	GPIOD
#define MCU_UART_TX_PIN		GPIO_PIN_5
#define MCU_UART_RX_PORT	GPIOD
#define MCU_UART_RX_PIN		GPIO_PIN_6

/* ================================================================
 * I2C0 (Master: CT7451 + PMBus)
 * ================================================================ */
#define I2C0_SCL_PORT		GPIOB
#define I2C0_SCL_PIN		GPIO_PIN_6
#define I2C0_SDA_PORT		GPIOB
#define I2C0_SDA_PIN		GPIO_PIN_7

/* ================================================================
 * I2C1 (Slave: 与 CV84X6 通信)
 * ================================================================ */
#define I2C1_SCL_PORT		GPIOB
#define I2C1_SCL_PIN		GPIO_PIN_10
#define I2C1_SDA_PORT		GPIOB
#define I2C1_SDA_PIN		GPIO_PIN_11

/* ================================================================
 * PWM (TIMER3 CH3)
 * ================================================================ */
#define PWM_VDDC_PORT		GPIOB
#define PWM_VDDC_PIN		GPIO_PIN_9

#endif /* __PIN_H__ */
