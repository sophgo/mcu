#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <common.h>
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

void dump(void *s, int len)
{
	int i;

	for (i = 0; i < len; ++i)
		debug("%02x ", ((uint8_t *)s)[i]);
	debug("\n");
}

#define ISL68224_SET_DMA_ADDR_CMD	0xc7
#define ISL68224_GET_DMA_DATA_CMD	0xc5
#define ISL68224_GET_DEVICE_ID_CMD	0xad
#define ISL68224_GET_REVERSION_ID_CMD	0xae

#define ISL68224_NVM_SLOT_NUM_REG	0xc2

int isl68224_set_dma_addr(int i2c, int slave_addr, uint16_t addr)
{
	uint8_t tmp[2];

	tmp[0] = addr & 0xff;
	tmp[1] = (addr >> 8) & 0xff;

	return i2c_smbus_write_i2c_block_data(i2c, slave_addr, ISL68224_SET_DMA_ADDR_CMD, sizeof(tmp), tmp);
}

int isl68224_get_dma_data(int i2c, int slave_addr, uint16_t addr, unsigned int len, uint8_t *data)
{
	int err;

	err = isl68224_set_dma_addr(i2c, slave_addr, addr);
	if (err) {
		debug("\r\nfailed to set dma address\r\n");
		return err;
	}

	return i2c_smbus_read_i2c_block_data(i2c, slave_addr, ISL68224_GET_DMA_DATA_CMD, len, data);
}

unsigned int isl68224_get_nvm_slot_num(int i2c, int addr)
{
	uint8_t tmp[4];
    int err;

	err = isl68224_get_dma_data(i2c, addr, ISL68224_NVM_SLOT_NUM_REG, sizeof(tmp), tmp);
    if (err) {
		debug("\r\nfailed to get dma data\r\n");
		return err;
	}
	return tmp[0] | (tmp[1] << 8) | (tmp[2] << 16) | (tmp[3] << 24);
}

unsigned int isl68224_get_device_id(int i2c, int addr)
{
	uint8_t tmp[5];
	int err;

	err = i2c_smbus_read_i2c_block_data(i2c, addr, ISL68224_GET_DEVICE_ID_CMD, sizeof(tmp), tmp);
    if (err) {
		debug("\r\nfailed to get device id\r\n");
		return err;
	}
	debug("device id:");
	dump(tmp, sizeof(tmp));

	return err;
}

unsigned int isl68224_get_reversion_id(int i2c, int addr)
{
	uint8_t tmp[5];
	int err;

	err = i2c_smbus_read_i2c_block_data(i2c, addr,
         ISL68224_GET_REVERSION_ID_CMD, sizeof(tmp), tmp);
    if (err) {
		debug("\r\nfailed to get reversion id\r\n");
		led_3_1_on();
		return err;
	}
    debug("reversion id:");
	dump(tmp, sizeof(tmp));
	led_3_0_on();

	return err;
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

int isl68224_program(int i2c, int addr, char *name)
{
	FILINFO info;
	int err;
	char *hex;
    FIL fp;
    UINT br;

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

    for (int i = 0; i < info.fsize; i++){
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
		goto free_buf;
	}

	hex[info.fsize] = 0;

	// for (int i = 0; i < info.fsize; i++){
    //     uart_putc(hex[i]);
    // }

	int i, j;
	int tag, nb, cmd, pa, crc;
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

#if 0
		debug("L: ");
		for (int i = 0; i < line_size; ++i)
			debug("%c", line_buf[i]);
		debug("\n");
#endif

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
		crc = data_buf[data_size - 1];

		debug("%02X", tag);
		debug("%02X", nb);
		debug("%02X", pa);
		debug("%02X", cmd);

		for (int i = 0; i < nb - 3; ++i) {
			debug("%02X", data_buf[4 + i]);
			write_buf[i] = data_buf[4 + i];
		}

		debug("%02X", crc);
		debug("\r\n");

		err = i2c_smbus_write_i2c_block_data(i2c, addr, cmd, nb - 3, write_buf);
		if(err)
			debug("i2c timeout\r\n");
	}

	led_4_1_on();

free_buf:
	free(hex);

	return 0;
}
