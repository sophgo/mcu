#include <libopencm3/stm32/gpio.h>
#include <stdlib.h>
#include <tick.h>
#include <i2c_master.h>

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

struct power_node {
	char *name;
	int check;
	int port, pin;
	int (*power_on)(void *ctx);
	void (*power_off)(void *ctx);
	void *ctx;
	unsigned long delay;
};
#define PMIC_SLAVE_ADDR 0x60

int pmic_buck_on(unsigned int buck)
{
	int tmp;
	tmp = i2c_master_smbus_read_byte(I2C2, PMIC_SLAVE_ADDR, 0x40);
	tmp |= (1 << 5) >> buck;
	i2c_master_smbus_write_byte(I2C2, PMIC_SLAVE_ADDR, 0x40, tmp);
	return 0;
}

void pmic_buck_off(unsigned int buck)
{
	int tmp;
	tmp = i2c_master_smbus_read_byte(I2C2, PMIC_SLAVE_ADDR, 0x40);
	tmp &= ~((1 << 5) >> buck);
	i2c_master_smbus_write_byte(I2C2, PMIC_SLAVE_ADDR, 0x40, tmp);
}

int pmic_voltage_config(unsigned int buck, unsigned int voltage)
{
	unsigned int reg_h = 0x13 + 8 * buck;
	unsigned int reg_l = 0x14 + 8 * buck;
	unsigned int value = voltage / 2;
	unsigned int val_h = (value >> 8) & 0x03;
	unsigned int val_l = value & 0xff;

	i2c_master_smbus_write_byte(I2C2, PMIC_SLAVE_ADDR, reg_h, val_h);
	i2c_master_smbus_write_byte(I2C2, PMIC_SLAVE_ADDR, reg_l, val_l);
	return 0;
}

int pmic_init(void *ctx)
{
	/* enable system enable, disable all buck */
	i2c_master_smbus_write_byte(I2C2, PMIC_SLAVE_ADDR, 0x40, 0x80);
	pmic_voltage_config(0, 1800);
	pmic_voltage_config(1, 600);
	pmic_voltage_config(2, 1100);
	pmic_voltage_config(3, 840);
	return 0;
}

void pmic_destroy(void *ctx)
{
#if 0
	/* disable system enable, disable all buck */
	i2c_master_smbus_write_byte(I2C2, PMIC_SLAVE_ADDR, 0x40, 0x00);
#endif
}

int p0v8_on(void *ctx)
{
	return pmic_buck_on(3);
}

void p0v8_off(void *ctx)
{
	pmic_buck_off(3);
}

int p1v1_on(void *ctx)
{
	return pmic_buck_on(1);
}

void p1v1_off(void *ctx)
{
	pmic_buck_off(1);
}

int p0v6_on(void *ctx)
{
	return pmic_buck_on(2);
}

void p0v6_off(void *ctx)
{
	pmic_buck_off(2);
}

int p1v8_on(void *ctx)
{
	return pmic_buck_on(0);
}

void p1v8_off(void *ctx)
{
	pmic_buck_off(0);
}

struct power_node power_sequence[] = {
	{"cfg-pmic", 0, 0, 0, pmic_init, pmic_destroy, NULL, 1},
	{"pwr-1.8v", 0, 0, 0, p1v8_on, p1v8_off, NULL, 1},
	{"en-io18", 0, GPIOB, GPIO6, NULL, NULL, NULL, 1},
	{"en-core", 0, GPIOB, GPIO12, NULL, NULL, NULL, 1},
	{"en-io33", 0, GPIOB, GPIO7, NULL, NULL, NULL, 1},
	{"pwr-0.8v", 0, 0, 0, p0v8_on, p0v8_off, NULL, 1},/* vdd_phy vdd_pcie */
	{"pg-p08", 0, GPIOA, GPIO11, NULL, NULL, NULL, 1},
	{"pg-pcie", 0, GPIOA, GPIO9, NULL, NULL, NULL, 1},
	{"en-tpu", 0, GPIOB, GPIO13, NULL, NULL, NULL, 1},
	{"ck-tpu", 1, GPIOA, GPIO4, NULL, NULL, NULL, 0},
	{"pg-tpu", 0, GPIOA, GPIO10, NULL, NULL, NULL, 1},
	{"pwr-1.1v", 0, 0, 0, p1v1_on, p1v1_off, NULL, 1}, /* ddr_vddq */
	{"pwr-0.6v", 0, 0, 0, p0v6_on, p0v6_off, NULL, 1}, /* ddr_vddlp */
	{"en-tpu-mem", 0, GPIOB, GPIO14, NULL, NULL, NULL, 1},
	{"ck-tpu-mem", 1, GPIOA, GPIO2, NULL, NULL, NULL, 0},
	{"pg-tpu-mem", 0, GPIOA, GPIO7, NULL, NULL, NULL, 1},
	{"en-vqps18", 0, GPIOA, GPIO15, NULL, NULL, NULL, 1},
	{"pg-ddr", 0, GPIOA, GPIO12, NULL, NULL, NULL, 1},
};

int node_on(struct power_node *node)
{
	int err = 0;

	if (node->check) {
		while (gpio_get(node->port, node->pin) == 0)
			;
	} else if (node->power_on) {
		err = node->power_on(node->ctx);
	} else {
		gpio_set(node->port, node->pin);
	}

	if (err == 0 && node->delay)
		mdelay(node->delay);

	return err;
}

void node_off(struct power_node *node)
{
	if (node->power_off)
		node->power_off(node->ctx);
	else
		gpio_clear(node->port, node->pin);
}

void sequence_init(struct power_node *node, int num)
{
	int i;

	for (i = 0; i < num; ++i) {
		if (node[i].power_on && node[i].power_off)
			continue;
		if (node[i].check) {
			gpio_mode_setup(node[i].port,
					GPIO_MODE_INPUT,
					GPIO_PUPD_PULLDOWN,
					node[i].pin);
		} else {
			gpio_mode_setup(node[i].port,
					GPIO_MODE_OUTPUT,
					GPIO_PUPD_PULLUP,
					node[i].pin);
			gpio_clear(node[i].port, node[i].pin);
		}
	}
}

int sequence_on(struct power_node *node, int num)
{
	int err, i;

	err = 0;

	for (i = 0; i < num; ++i) {
		err = node_on(node + i);
		if (err)
			break;
	}
	if (i != num) {
		--i;
		while (i >= 0)
			node_off(node + i);
	}
	return err;
}

void sequence_off(struct power_node *node, int num)
{
	int i;

	for (i = num; i >= 0; --i)
		node_off(node + i);
}

void power_init(void)
{
	sequence_init(power_sequence, ARRAY_SIZE(power_sequence));
}

int power_on(void)
{
	return sequence_on(power_sequence, ARRAY_SIZE(power_sequence));
}

void power_off(void)
{
	sequence_off(power_sequence, ARRAY_SIZE(power_sequence));
}
