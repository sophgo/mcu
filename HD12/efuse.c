#include <dbgi2c.h>
#include <stdint.h>
#include <stdbool.h>
#include <tick.h>
#include <system.h>

#define GPIO_SWPORTA_DR 0x00
#define GPIO_SWPORTA_DDR 0x04

#define cpu_write32(chip, addr, data)	dbgi2c_write32(chip, addr, data)
#define cpu_read32(chip, addr, data)		dbgi2c_read32(chip, addr, data)

#define NUM_ADDRESS_BITS    8

#define EFUSE0_BASE     0x7040000000UL
#define EFUSE1_BASE     0x7040001000UL
#define GPIO_BASE_ADDR  0x7040009000UL
#define ID_BASE_ADDR    0xf8 //vendor id && device id
#define EFUSE_ENABLE_ADDR  0x0

#define EFUSE0_MODE     EFUSE0_BASE
#define EFUSE1_MODE     EFUSE1_BASE
#define EFUSE0_ADR      (EFUSE0_BASE + 0x4)
#define EFUSE1_ADR      (EFUSE1_BASE + 0x4)
#define EFUSE0_RD_DATA  (EFUSE0_BASE + 0xc)
#define EFUSE1_RD_DATA  (EFUSE1_BASE + 0xc)

static uint32_t make_adr_val(uint32_t address, uint32_t bit_i)
{
	const uint32_t address_mask = (1 << NUM_ADDRESS_BITS) - 1;

	return (address & address_mask) |
		((bit_i & 0x1f) << NUM_ADDRESS_BITS);
}

static void efuse0_mode_wait_ready(int chip)
{
    uint32_t mode = 0;
    while ((mode & 0b11) != 0) {
        cpu_read32(chip, EFUSE0_MODE, &mode);
    }
}

static void efuse1_mode_wait_ready(int chip)
{
    uint32_t mode = 0;
    while ((mode & 0b11) != 0) {
        cpu_read32(chip, EFUSE1_MODE, &mode);
    }
}

static void efuse0_mode_md_write(int chip, uint32_t val)
{
    uint32_t mode = 0;
    uint32_t new = 0;

    mode = cpu_read32(chip, EFUSE0_MODE, &mode);
    new = (mode & 0xfffffffc) | (val & 0b11);
    cpu_write32(chip, EFUSE0_MODE, new);
}

static void efuse1_mode_md_write(int chip, uint32_t val)
{
    uint32_t mode = 0;
    uint32_t new = 0;
    
    mode = cpu_read32(chip, EFUSE1_MODE, &mode);
    new = (mode & 0xfffffffc) | (val & 0b11);
    cpu_write32(chip, EFUSE1_MODE, new);
}

static void efuse0_set_bit(int chip, uint32_t address, uint32_t bit_i)
{
	uint32_t adr_val = make_adr_val(address, bit_i);

    cpu_write32(chip, EFUSE0_ADR, adr_val);
    efuse0_mode_md_write(chip, 0b11);
    efuse0_mode_wait_ready(chip);
}

static void efuse1_set_bit(int chip, uint32_t address, uint32_t bit_i)
{
	uint32_t adr_val = make_adr_val(address, bit_i);

    cpu_write32(chip, EFUSE1_ADR, adr_val);
    efuse1_mode_md_write(chip, 0b11);
    efuse1_mode_wait_ready(chip);
}

static void efuse0_mode_reset(int chip)
{
    cpu_write32(chip, EFUSE0_MODE, 0);
    efuse0_mode_wait_ready(chip);
}

static void efuse1_mode_reset(int chip)
{
    cpu_write32(chip, EFUSE1_MODE, 0);
    efuse1_mode_wait_ready(chip);
}

static void efuse_set_volt(int chip, uint64_t gpio_base, int is_high)
{
    uint32_t dr_value = 0;
    uint32_t ddr_value = 0;
	if (is_high) {
		// pull up gpio18 for efuse burn
        cpu_read32(chip, gpio_base + GPIO_SWPORTA_DR, &dr_value);
		cpu_write32(chip, gpio_base + GPIO_SWPORTA_DR, dr_value | (1 << 28));

        cpu_read32(chip, gpio_base + GPIO_SWPORTA_DDR, &ddr_value);
        cpu_write32(chip, gpio_base + GPIO_SWPORTA_DDR, ddr_value | (1 << 28));
	} else {
        cpu_read32(chip, gpio_base + GPIO_SWPORTA_DR, &dr_value);
        cpu_write32(chip, gpio_base + GPIO_SWPORTA_DR, dr_value & ~(1 << 28));

        cpu_read32(chip, gpio_base + GPIO_SWPORTA_DDR, &ddr_value);
        cpu_write32(chip, gpio_base + GPIO_SWPORTA_DDR, ddr_value | (1 << 28));
    }
}

static void efuse0_embedded_write(int chip, uint64_t addr, uint32_t value)
{
    for (int i = 0; i < 32; i++)
        if ((value >> i) & 1)
            efuse0_set_bit(chip, addr, i);
}

static void efuse1_embedded_write(int chip, uint64_t addr, uint32_t value)
{
    for (int i = 0; i < 32; i++)
        if ((value >> i) & 1)
            efuse1_set_bit(chip, addr, i);
}

static uint32_t efuse0_embedded_read(int chip, uint64_t addr)
{
    uint32_t adr_val;
	uint32_t ret =  0xdeadbeef;

    efuse0_mode_reset(chip);
    adr_val = make_adr_val(addr, 0);
    cpu_write32(chip, EFUSE0_ADR, adr_val);
    efuse0_mode_md_write(chip, 0b10);
    efuse0_mode_wait_ready(chip);
    cpu_read32(chip, EFUSE0_RD_DATA, &ret);

    return ret;
}

static uint32_t efuse1_embedded_read(int chip, uint64_t addr)
{
    uint32_t adr_val;
	uint32_t ret = 0xdeadbeef;

    efuse1_mode_reset(chip);
    adr_val = make_adr_val(addr, 0);
    cpu_write32(chip, EFUSE1_ADR, adr_val);
    efuse1_mode_md_write(chip, 0b10);
    efuse1_mode_wait_ready(chip);
    cpu_read32(chip, EFUSE1_RD_DATA, &ret);

    return ret;
}


uint32_t efuse_read32(int chip, int efuse_id)
{
    unsigned int val_r = 0xdeadbeef;

    if (efuse_id == 0) {
        val_r = efuse0_embedded_read(chip, ID_BASE_ADDR);
    } else if (efuse_id == 1) {
        val_r = efuse1_embedded_read(chip, ID_BASE_ADDR);
    }
	return val_r;
}

int efuse_write32(int chip, int efuse_id, uint32_t value)
{
    uint32_t val_r = 0xdeadbeef;
    int ret = 0;

    if (efuse_id == 0) {
        efuse_set_volt(chip, GPIO_BASE_ADDR, true);
        mdelay(1);
        efuse0_embedded_write(chip, ID_BASE_ADDR, value);
        efuse_set_volt(chip, GPIO_BASE_ADDR, false);

        val_r = efuse0_embedded_read(chip, ID_BASE_ADDR);
        dbg_printf("chip-%d, efuse-%d val:0x%lx\n", chip, efuse_id, val_r);

        if (val_r != value) {
            dbg_printf("chip-%d, efuse-%d write failed, keep origin\n", chip, efuse_id);
            return -1;
        } else {
            dbg_printf("chip-%d, efuse-%d write success\n", chip, efuse_id);
        }
    } else if (efuse_id == 1) {
        efuse_set_volt(chip, GPIO_BASE_ADDR, true);
        mdelay(1);
        efuse1_embedded_write(chip, ID_BASE_ADDR, value);
        efuse_set_volt(chip, GPIO_BASE_ADDR, false);

        val_r = efuse1_embedded_read(chip, ID_BASE_ADDR);
        dbg_printf("chip-%d, efuse-%d val:0x%lx\n", chip, efuse_id, val_r);

        if (val_r != value) {
            dbg_printf("chip-%d, efuse-%d write failed, keep origin\n", chip, efuse_id);
            return -1;
        } else {
            dbg_printf("chip-%d, efuse-%d write success\n", chip, efuse_id);
        }
    }

    /* enable new config */
    efuse_set_volt(chip, GPIO_BASE_ADDR, true);
    mdelay(1);
    efuse0_embedded_write(chip, EFUSE_ENABLE_ADDR, 0x1);
    efuse_set_volt(chip, GPIO_BASE_ADDR, false);

    val_r = efuse0_embedded_read(chip, EFUSE_ENABLE_ADDR);
    dbg_printf("chip-%d efuse-%d val:0x%lx\n", chip, efuse_id, val_r);

	return ret;
}