#include <common.h>
#include <loop.h>
#include <pin.h>
static void shifter_process(void)
{
	if (gpio_input_bit_get(PCIE_RRST_IN_PORT, PCIE_RRST_IN_PIN))
		gpio_bit_set(PCIE_RRST_OUT_PORT, PCIE_RRST_OUT_PIN);
	else
		gpio_bit_reset(PCIE_RRST_OUT_PORT, PCIE_RRST_OUT_PIN);
}

void shifter_init(void)
{
	loop_add(shifter_process);
}
