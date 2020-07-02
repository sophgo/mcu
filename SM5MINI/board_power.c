#include <power.h>
#include <common.h>
#include <mp5475.h>
#include <pin.h>
#include <timer.h>

static int io18_on(void)
{
	return mp5475_buck_on(0);
}

static int io18_off(void)
{
	mp5475_buck_off(0);
	return 0;
}

static int pcie_on(void)
{
	return mp5475_buck_on(3);
}

static int pcie_off(void)
{
	mp5475_buck_off(3);
	return 0;
}

static int q_on(void)
{
	return mp5475_buck_on(1);
}

static int q_off(void)
{
	mp5475_buck_off(1);
	return 0;
}

static int qlp_on(void)
{
	return mp5475_buck_on(2);
}

static int qlp_off(void)
{
	mp5475_buck_off(2);
	return 0;
}

static int sys_on(void)
{
	gpio_set(SYS_RST_PORT, SYS_RST_PIN);
	timer_mdelay(30);
	gpio_clear(SYS_RST_PORT, SYS_RST_PIN);
	timer_mdelay(30);
	gpio_set(PG_DDR_PORT, PG_DDR_PIN);
	return 0;
}

static int sys_off(void)
{
	gpio_clear(SYS_RST_PORT, SYS_RST_PIN);
	gpio_clear(PG_DDR_PORT, PG_DDR_PIN);
	return 0;
}

#define PWR_FUNC(name, func, delay)			\
	{						\
		name, NODE_TYPE_FUNCTION, delay,	\
		{					\
			(unsigned long)(func ## _on),	\
			(unsigned long)(func ## _off)	\
		}					\
	}

#define PWR_GPIO(name, io, delay)			\
	{						\
		name, NODE_TYPE_ENABLE, delay,		\
		{					\
			(unsigned long)(io ## _PORT),	\
			(unsigned long)(io ## _PIN)	\
		}					\
	}

static struct power_node power_chain[] = {
	PWR_FUNC("bulk0-io-1.8", io18, 1000),
	PWR_GPIO("en-io-1.8", EN_VDD_IO18, 1000),
	PWR_GPIO("en-core", EN_VDD_C, 1000),
	PWR_GPIO("en-io-3.3", EN_VDD_IO33, 1000),
	PWR_FUNC("bulk3-pcie", pcie, 1000),
	PWR_GPIO("pg-0.8", PG_0P8, 1000),
	PWR_GPIO("pg-pcie", PG_PCIE, 1000),
	PWR_GPIO("en-tpu", EN_VDD_TPU, 1000),
	PWR_GPIO("pg-tpu", PG_VDD_TPU, 1000),
	PWR_FUNC("bulk1-q", q, 1000),
	PWR_FUNC("bulk1-qlp", qlp, 1000),
	PWR_GPIO("en-tpu-mem", EN_VDD_TPU_MEM, 1000),
	PWR_GPIO("pg-tpu-mem", PG_VDD_TPU_MEM, 1000),
	PWR_GPIO("en-vqps-1.8", EN_VQPS18, 1000),
	PWR_FUNC("sys-rst", sys, 1000),
};

int board_power_on(void)
{
	return power_on(power_chain, ARRAY_SIZE(power_chain));
}

void board_power_off(void)
{
	power_off(power_chain, ARRAY_SIZE(power_chain));
}
