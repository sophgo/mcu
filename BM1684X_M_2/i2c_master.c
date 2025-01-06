#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>
#include <tick.h>
#include <debug.h>
#include <timer.h>

int i2c_master_init(int i2c)
{
	i2c_reset(i2c);
	i2c_peripheral_disable(i2c);
	//configure ANFOFF DNF[3:0] in CR1
	i2c_enable_analog_filter(i2c);
	i2c_set_digital_filter(i2c, 0);

	i2c_set_speed(i2c, i2c_speed_sm_100k, rcc_apb1_frequency / 1000 / 1000);

	//configure No-Stretch CR1 (only relevant in slave mode)
	i2c_enable_stretching(i2c);
	//addressing mode
	i2c_set_7bit_addr_mode(i2c);
	i2c_peripheral_enable(i2c);
	return 0;
}

int i2c_master_destroy(int i2c)
{
	i2c_peripheral_disable(i2c);
	return 0;
}

int i2c_transfer7_timeout(uint32_t i2c, uint8_t addr, unsigned int timeout,
			   uint8_t *w, size_t wn,
			   uint8_t *r, size_t rn)
{
	timer_start(timeout * 1000);

	/*  waiting for busy is unnecessary. read the RM */
	if (wn) {
		i2c_set_7bit_address(i2c, addr);
		i2c_set_write_transfer_dir(i2c);
		i2c_set_bytes_to_transfer(i2c, wn);
		if (rn) {
			i2c_disable_autoend(i2c);
		} else {
			i2c_enable_autoend(i2c);
		}
		i2c_send_start(i2c);

		while (wn--) {
			bool wait = true;
			while (wait) {
				if (i2c_transmit_int_status(i2c)) {
					wait = false;
				}
				if (timer_is_timeout())
					goto i2c_timeout;
				while (i2c_nack(i2c))
					if (timer_is_timeout())
						goto i2c_timeout;
			}
			i2c_send_data(i2c, *w++);
		}
		/* not entirely sure this is really necessary.
		 * RM implies it will stall until it can write out the later bits
		 */
		if (rn) {
			while (!i2c_transfer_complete(i2c))
				if (timer_is_timeout())
					goto i2c_timeout;
		}
	}

	if (rn) {
		/* Setting transfer properties */
		i2c_set_7bit_address(i2c, addr);
		i2c_set_read_transfer_dir(i2c);
		i2c_set_bytes_to_transfer(i2c, rn);
		/* start transfer */
		i2c_send_start(i2c);
		/* important to do it afterwards to do a proper repeated start! */
		i2c_enable_autoend(i2c);

		for (size_t i = 0; i < rn; i++) {
			while (i2c_received_data(i2c) == 0)
				if (timer_is_timeout())
					goto i2c_timeout;
			r[i] = i2c_get_data(i2c);
		}
	}

	/* wait stop condition */
	while (i2c_busy(i2c))
		if (timer_is_timeout())
			goto i2c_timeout;

	timer_stop();

	return 0;
i2c_timeout:
	i2c_peripheral_disable(i2c);
	asm volatile ("nop; nop; nop; nop");
	i2c_peripheral_enable(i2c);
	return -1;
}

int i2c_master_trans(int i2c, unsigned char addr, unsigned long timeout,
		     void *w, unsigned int wn,
		     void *r, unsigned int rn)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, w, wn, r, rn);
}

int i2c_master_smbus_read_byte(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, unsigned char *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, &cmd, 1, data, 1);
}

int i2c_master_smbus_write_byte(int i2c, unsigned char addr,
				unsigned long timeout,
				unsigned char cmd, unsigned char data)
{
	unsigned char tmp[2];

	tmp[0] = cmd;
	tmp[1] = data;
	return i2c_transfer7_timeout(i2c, addr, timeout, tmp, 2, NULL, 0);
}

int i2c_master_smbus_write(int i2c, unsigned char addr,
			   unsigned long timeout, unsigned char data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, &data, 1, NULL, 0);
}

int i2c_master_smbus_read(int i2c, unsigned char addr,
			  unsigned long timeout, unsigned char *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, NULL, 0, data, 1);
}

int i2c_master_smbus_read_word(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, uint16_t *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout,
				     &cmd, 1, (uint8_t *)data, 2);
}


int i2c_master_smbus_write_word(int i2c, unsigned char addr,
				unsigned long timeout,
				unsigned char cmd, uint16_t data)
{
	unsigned char tmp[3];

	tmp[0] = cmd;
	tmp[1] = data & 0xff;
	tmp[2] = (data >> 8) & 0xff;

	return i2c_transfer7_timeout(i2c, addr, timeout, tmp, 3, NULL, 0);
}
