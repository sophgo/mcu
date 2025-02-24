#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <common.h>
#include <timer.h>
#include <i2c.h>
#include <debug.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include <fat32/include/ff.h>

static struct
{
	int num;
	uint8_t set_dma_addr_cmd;
	uint8_t get_dma_data_cmd;
	uint8_t get_device_id_cmd;
	uint8_t get_reversion_id_cmd;
	uint16_t nvm_slot_num_reg;
} device_info[] = {
	{0, 0xC7, 0xC5, 0xAD, 0xAE, 0xC2},
	{1, 0xF7, 0xF5, 0xAD, 0xAE, 0x1080},
};

static void dump(void *s, int len)
{
	int i;

	for (i = 0; i < len; ++i)
		debug("%02x ", ((uint8_t *)s)[i]);
	debug("\n");
}

static int hex2bin(char hex)
{
	if (hex >= '0' && hex <= '9')
		return hex - '0';
	else if (hex >= 'a' && hex <= 'f')
		return hex - 'a' + 10;
	else if (hex >= 'A' && hex <= 'F')
		return hex - 'A' + 10;
	else
		return -EINVAL;
}

static int isl68224_set_dma_addr(uint16_t addr)
{
	uint8_t tmp[2];
	int id;
	uint8_t slave_addr;

	id = get_default_device();
	slave_addr = get_default_addr();
	tmp[0] = addr & 0xff;
	tmp[1] = (addr >> 8) & 0xff;

	return i2c_smbus_write_i2c_block_data(SLAVE_I2C, slave_addr, device_info[id].set_dma_addr_cmd, sizeof(tmp), tmp);
}

static int isl68224_get_dma_data(uint16_t addr, unsigned int len, uint8_t *data)
{
	int err;
	int id;
	uint8_t slave_addr;

	id = get_default_device();
	slave_addr = get_default_addr();
	err = isl68224_set_dma_addr(addr);
	if (err) {
		debug("\r\nfailed to set dma address\r\n");
		return err;
	}

	return i2c_smbus_read_i2c_block_data(SLAVE_I2C, slave_addr, device_info[id].get_dma_data_cmd, len, data);
}

void isl68224_get_nvm_slot_num(void)
{
	uint8_t tmp[4];
    int err;
	int id;

	for (err = 0; err < 4; err++)
		tmp[err] = 0;
	id = get_default_device();
	err = isl68224_get_dma_data(device_info[id].nvm_slot_num_reg, sizeof(tmp), tmp);
    if (err) {
		debug("\r\nfailed to get dma data\r\n");
		return;
	}
	printf("i2c device's availiable nvm slots: %u\r\n", 
		 (unsigned int)(tmp[0] | (tmp[1] << 8) | (tmp[2] << 16) | (tmp[3] << 24)));
}

void isl68224_get_device_id(void)
{
	uint8_t tmp[5];
	int err;
	int id;
	uint8_t slave_addr;

	id = get_default_device();
	slave_addr = get_default_addr();
	err = i2c_smbus_read_i2c_block_data(SLAVE_I2C, slave_addr,
		 device_info[id].get_device_id_cmd, sizeof(tmp), tmp);
    if (err) {
		debug("\r\nfailed to get device id\r\n");
		return;
	}
	debug("device id:");
	dump(tmp, sizeof(tmp));
}

void isl68224_get_reversion_id()
{
	uint8_t tmp[5];
	int err;
	int id;
	uint8_t slave_addr;

	id = get_default_device();
	slave_addr = get_default_addr();
	err = i2c_smbus_read_i2c_block_data(SLAVE_I2C, slave_addr,
         device_info[id].get_reversion_id_cmd, sizeof(tmp), tmp);
    if (err) {
		debug("\r\nfailed to get reversion id\r\n");
		led_3_1_on();
		return;
	}
    debug("reversion id:");
	dump(tmp, sizeof(tmp));
	led_3_0_on();
}

int isl68224_proof(char *name)
{
	static FILINFO info;
	int err;
	char *hex;
	FIL fp;
	UINT br;
	uint8_t slave_addr;

	err = f_stat(name, &info);
	if (err) {
		debug("\r\ncannot get file status\r\n");
		return err;
	}

	hex = malloc(info.fsize + 1);
	if (!hex) {
		debug("\r\nallocate buffer failed\r\n");
		return -ENOMEM;
	}

	for (int i = 0; i < info.fsize; i++) {
		hex[i] = '?';
	}

	err = f_open(&fp, name, FA_READ);
	if (err != 0) {
		debug("\r\ncannot open file %s\r\n", name);
		return err;
	}

	err = f_read(&fp, hex, info.fsize, &br);
	f_close(&fp);
	if (br < info.fsize) {
		debug("\r\ncannot get enough data\r\n");
		return err;
	}

	hex[info.fsize] = 0;

	int i, j;
	// int tag, nb, cmd, pa, crc;
	int tag, nb, pa;
	uint8_t line_buf[64];
	int line_size;
	uint8_t data_buf[32];
	int data_size;

	for (i = 0; i < info.fsize; ++i) {
		for (line_size = 0; i < info.fsize; ++i) {
			if (line_size > sizeof(line_buf)) {
				debug("\r\nhex string too long\r\n");
				goto free_buf;
			}

			if (hex[i] == '\n')
				break;
			else if (isxdigit((int)hex[i]))
				line_buf[line_size++] = hex[i];
		}

		/* empty line */
		if (line_size == 0)
			continue;

		if (line_size % 2) {
			debug("\r\nwrong hex number per line\r\n");
			goto free_buf;
		}

		/* convert hex string to binary */
		for (j = 0; j < line_size; j += 2) {
			data_buf[j / 2] = hex2bin(line_buf[j]) << 4 |
				hex2bin(line_buf[j + 1]);
		}
		data_size = line_size / 2;

		if (data_size < 2) {
			debug("\r\nwrong format: line too short\r\n");
			goto free_buf;
		}

		tag = data_buf[0];
		nb = data_buf[1];

		if (nb + 2 != data_size) {
			debug("\r\nwrong format: invalid byte count\r\n");
			goto free_buf;
		}

		if (tag != 0)
			/* skip lines that shouldnot burn into device */
			continue;

		if (nb < 3) {
			debug("\r\nwrong format: invalid byte count\r\n");
			goto free_buf;
		}

		pa = data_buf[2];
		slave_addr = pa >> 1;
		if (get_default_addr() != slave_addr){
			set_default_addr(slave_addr);
			set_default_device(!get_default_device());
			debug("default device has been proofed\r\n");
		}
		else{
			debug("default device is correct\r\n");
		}
		break;
	}

free_buf:
	free(hex);

	return 0;
}

int isl68224_program(char *name)
{
	static FILINFO info;
	int err;
	char *hex;
	FIL fp;
	UINT br;
	uint8_t slave_addr;

	err = f_stat(name, &info);
	if (err) {
		debug("\r\ncannot get file status\r\n");
		led_2_1_on();
		return err;
	}

	hex = malloc(info.fsize + 1);

	debug("infosize=%lu\n",info.fsize);
	if (!hex) {
		debug("\r\nallocate buffer failed\r\n");
		led_2_1_on();
		return -ENOMEM;
	}

	for (int i = 0; i < info.fsize; i++) {
		hex[i] = '?';
	}

	err = f_open(&fp, name, FA_READ);
	if (err != 0) {
		debug("\r\ncannot open file %s\r\n", name);
		led_2_1_on();
		return err;
	}

	err = f_read(&fp, hex, info.fsize, &br);
	f_close(&fp);
	if (br < info.fsize) {
		debug("\r\ncannot get enough data\r\n");
		led_2_1_on();
		return err;
	}

	hex[info.fsize] = 0;

	int i, j;
	// int tag, nb, cmd, pa, crc;
	int tag, nb, cmd, pa;
	uint8_t line_buf[64];
	int line_size;
	uint8_t data_buf[32];
	uint8_t write_buf[32];
	int data_size;

	for (i = 0; i < info.fsize; ++i) {
		for (line_size = 0; i < info.fsize; ++i) {
			if (line_size > sizeof(line_buf)) {
				debug("\r\nhex string too long\r\n");
				goto free_buf;
			}

			if (hex[i] == '\n')
				break;
			else if (isxdigit((int)hex[i]))
				line_buf[line_size++] = hex[i];
		}

		/* empty line */
		if (line_size == 0)
			continue;

		if (line_size % 2) {
			debug("\r\nwrong hex number per line\r\n");
			goto free_buf;
		}

		/* convert hex string to binary */
		for (j = 0; j < line_size; j += 2) {
			data_buf[j / 2] = hex2bin(line_buf[j]) << 4 |
				hex2bin(line_buf[j + 1]);
		}
		data_size = line_size / 2;

		if (data_size < 2) {
			debug("\r\nwrong format: line too short\r\n");
			goto free_buf;
		}

		tag = data_buf[0];
		nb = data_buf[1];

		if (nb + 2 != data_size) {
			debug("\r\nwrong format: invalid byte count\r\n");
			goto free_buf;
		}

		if (tag != 0)
			/* skip lines that shouldnot burn into device */
			continue;

		if (nb < 3) {
			debug("\r\nwrong format: invalid byte count\r\n");
			goto free_buf;
		}

		pa = data_buf[2];
		cmd = data_buf[3];

		/* ignore, we donnot use PEC */
		// crc = data_buf[data_size - 1];

		// debug("%02X", tag);
		// debug("%02X", nb);
		// debug("%02X", pa);
		// debug("%02X", cmd);

		for (int k = 0; k < nb - 3; ++k) {
			// debug("%02X", data_buf[4 + k]);
			write_buf[k] = data_buf[4 + k];
		}

		// debug("%02X", crc);
		debug("  programming... %ld%%\r", (i + 1) * 100 / info.fsize);

		slave_addr = pa >> 1;
		if (get_default_addr() != slave_addr){
 			set_default_addr(slave_addr);
			set_default_device(!get_default_device());
		}
		err = i2c_smbus_write_i2c_block_data(SLAVE_I2C, slave_addr, cmd, nb - 3, write_buf);
		if(err) {
			led_3_1_on();
			debug("\ni2c timeout, burn failed\r\n");
			return err;
		}
	}

	timer_mdelay(1000);
	led_3_0_on();
	debug("\nburn successfully!\n");

free_buf:
	free(hex);

	return 0;
}

int isl68224_program_from_mcuflash(uint32_t file_addr)
{
	int i, j;
	int err = 1;
	//int tag, nb, cmd, pa, crc;
	int tag, nb, cmd, pa;
	uint8_t line_buf[64];
	int line_size;
	uint32_t info_fsize = 10000;
	uint8_t data_buf[32];
	uint8_t write_buf[32];
	int data_size;
	uint8_t slave_addr;

	for (i = 0; i < info_fsize; ++i) {
		for (line_size = 0; i < info_fsize; ++i) {
			if (line_size > sizeof(line_buf)) {
				debug("\r\nhex string too long\r\n");
				return err;
			}

			if ((*(volatile uint8_t *)(uint32_t)(file_addr + i)) == '\n')
				break;
			else if (isxdigit((*(volatile uint8_t *)(uint32_t)(file_addr + i))))
				line_buf[line_size++] = (*(volatile uint8_t *)(uint32_t)(file_addr + i));
		}

		/* empty line */
		if (line_size == 0)
			continue;

		if (line_size % 2) {
			debug("\r\nwrong hex number per line\r\n");
			return err;
		}

		/* convert hex string to binary */
		for (j = 0; j < line_size; j += 2) {
			data_buf[j / 2] = hex2bin(line_buf[j]) << 4 |
				hex2bin(line_buf[j + 1]);
		}
		data_size = line_size / 2;

		if (data_size < 2) {
			debug("\r\nwrong format: line too short\r\n");
			return err;
		}

		tag = data_buf[0];
		nb = data_buf[1];

		if (nb + 2 != data_size) {
			debug("\r\nwrong format: invalid byte count, nb+2=%d, size=%d\r\n", nb+2, data_size);
			return err;
		}

		if (tag != 0)
			/* skip lines that shouldnot burn into device */
			continue;

		if (nb < 3) {
			debug("\r\nwrong format: invalid byte count\r\n");
			return err;
		}

		pa = data_buf[2];
		cmd = data_buf[3];

		/* ignore, we donnot use PEC */
		// crc = data_buf[data_size - 1];

		// debug("%02X", tag);
		// debug("%02X", nb);
		// debug("%02X", pa);
		// debug("%02X", cmd);

		for (int i = 0; i < nb - 3; ++i) {
			//debug("%02X", data_buf[4 + i]);
			write_buf[i] = data_buf[4 + i];
		}

		// debug("%02X", crc);
		// debug("\r\n");
		debug("programming... %ld%%\r", (i + 1) * 100 / info_fsize);

		slave_addr = pa >> 1;
		if (get_default_addr() != slave_addr){
			set_default_addr(slave_addr);
			set_default_device(!get_default_device());
		}

		err = i2c_smbus_write_i2c_block_data(SLAVE_I2C, slave_addr, cmd, nb - 3, write_buf);
		if(err) {
			// led_3_1_on();
			// debug("i2c timeout\r\n");
			// return err;
		}
	}

	timer_mdelay(1000);
	led_3_0_on();
	debug("programming... 100%%\r");
	debug("\nburn successfully!\n");

	return err;
}