/*
 * system.c: CV84X6EVB 系统初始化
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_usart.h>
#include <gd32e50x_gpio.h>
#include <gd32e50x_rcu.h>
#include <gd32e50x_i2c.h>
#include <gd32e50x_misc.h>
#include <pin.h>
#include <debug.h>

static void system_gpio_init(void)
{
	/* 使能 GPIO 时钟 */
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);

	/* 输出引脚默认低电平 */
	gpio_bit_reset(PG_IND_PORT, PG_IND_PIN);
	gpio_bit_reset(PCIE_SEL_PORT, PCIE_SEL_PIN);
	gpio_bit_reset(EN_12V0_PORT, EN_12V0_PIN);
	gpio_bit_reset(POWEREN1_PORT, POWEREN1_PIN);
	gpio_bit_reset(POWEREN2_PORT, POWEREN2_PIN);
	gpio_bit_reset(POWEREN3_PORT, POWEREN3_PIN);
	gpio_bit_reset(SYS_RSTN_H_PORT, SYS_RSTN_H_PIN);
	gpio_bit_reset(PWR_RSTN_H_PORT, PWR_RSTN_H_PIN);
	gpio_bit_reset(PWR_BUTTON1_H_PORT, PWR_BUTTON1_H_PIN);
	gpio_bit_reset(PWR_ON_H_PORT, PWR_ON_H_PIN);
	gpio_bit_reset(PWR_WAKEUP_H_PORT, PWR_WAKEUP_H_PIN);
	gpio_bit_reset(PWM_VDDC_PORT, PWM_VDDC_PIN);

	/* LED 推挽输出 */
	gpio_init(PG_IND_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PG_IND_PIN);

	/* GPIO 控制输出 */
	gpio_init(PCIE_SEL_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PCIE_SEL_PIN);
	gpio_init(EN_12V0_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, EN_12V0_PIN);
	gpio_init(POWEREN1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, POWEREN1_PIN);
	gpio_init(POWEREN2_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, POWEREN2_PIN);
	gpio_init(POWEREN3_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, POWEREN3_PIN);
	gpio_init(SYS_RSTN_H_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, SYS_RSTN_H_PIN);
	gpio_init(PWR_RSTN_H_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PWR_RSTN_H_PIN);
	gpio_init(PWR_BUTTON1_H_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PWR_BUTTON1_H_PIN);
	gpio_init(PWR_ON_H_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PWR_ON_H_PIN);
	gpio_init(PWR_WAKEUP_H_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PWR_WAKEUP_H_PIN);

	/* Power Good 检测引脚，输入上拉 */
	gpio_init(PG_VDD_CORE_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, PG_VDD_CORE_PIN);
	gpio_init(DDR_VDD_PG_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, DDR_VDD_PG_PIN);
	gpio_init(DDR_VDDQ_PG_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, DDR_VDDQ_PG_PIN);
	gpio_init(PG_5V_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, PG_5V_PIN);
}

static void clock_init(void)
{
	/* HXTAL 8MHz -> PLL 200MHz, SYSCLK = 200MHz, HCLK = 100MHz, APB1 = 50MHz, APB2 = 100MHz */
	rcu_osci_on(RCU_HXTAL);
	while (rcu_osci_stab_wait(RCU_HXTAL) != SUCCESS);

	rcu_pll_config(RCU_PLLSRC_HXTAL_IRC48M, RCU_PLL_MUL25);
	rcu_osci_on(RCU_PLL_CK);
	while (rcu_osci_stab_wait(RCU_PLL_CK) != SUCCESS);

	rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);
	while (rcu_system_clock_source_get() != RCU_SCSS_PLL);

	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV2);
	rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV2);
	rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);
}

static void system_uart_init(void)
{
	rcu_periph_clock_enable(RCU_USART2);
	rcu_periph_clock_enable(RCU_GPIOD);

	/* PD5 = USART2_TX, PD6 = USART2_RX */
	gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

	usart_deinit(USART2);
	usart_baudrate_set(USART2, 115200);
	usart_word_length_set(USART2, USART_WL_8BIT);
	usart_stop_bit_set(USART2, USART_STB_1BIT);
	usart_parity_config(USART2, USART_PM_NONE);
	usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
	usart_receive_config(USART2, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
	usart_enable(USART2);
}

void system_i2c0_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_I2C0);

	/* PB6 = I2C0_SCL, PB7 = I2C0_SDA */
	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);

	i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
	i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
	i2c_enable(I2C0);
}

void system_i2c1_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_I2C1);

	/* PB10 = I2C1_SCL, PB11 = I2C1_SDA */
	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);

	/* I2C1 配置为 slave，在 slave_init() 中完成具体配置 */
}

void uart_putc(char c)
{
	while (usart_flag_get(USART2, USART_FLAG_TBE) == RESET);
	usart_data_transmit(USART2, (uint8_t)c);
}

char uart_getc(void)
{
	while (usart_flag_get(USART2, USART_FLAG_RBNE) == RESET);
	return (char)usart_data_receive(USART2);
}

void system_init(void)
{
	clock_init();
	system_gpio_init();
	system_uart_init();
	system_i2c0_init();
	system_i2c1_init();
}