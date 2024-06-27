#include <stdlib.h>
#include <stdbool.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <global.h>
#include <md5.h>
#include <i2c.h>
#include <project.h>

#define __maybe_unused		__attribute__((unused))

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof((array)[0]))

#define FILE_SIZE_64KB		0
#define FILE_SIZE_80KB		1

#define FWINFO_START(size)	((size) - 128)

#define EFIE_TYPE_APP		0

#define REG_BOARD_TYPE		(0x00)
#define REG_MCU_FAMILY		(0x18)
#define REG_CMD			(0x03)

#define REG_FLASH_CMD		(0x63)
#define REG_FLASH_OFFSET	(0x7c)
#define REG_FLASH_DATA		(0x80)
#define REG_FLASH_FLUSH		(0xff)

#define FLASH_CMD_UNLOCK	(0x02)
#define FLASH_CMD_LOCK		(0x03)
#define FLASH_CMD_ERASE		(0x04)

#define FLASH_PAGE_SIZE		(8 * 1024)
#define FLASH_PAGE_MASK		(FLASH_PAGE_SIZE - 1)

#define ROUND_UP(x, align)	(((x) + ((align) - 1)) / (align) * (align))
#define ROUND_DOWN(x, align)	((x) / (align) * (align))

static int file_type;

static struct {
	uint32_t	flash_size;
	uint32_t	elif_start;
	uint32_t	app_start;
	uint32_t	program_limit;
} file_info[] = {
	{(64 * 1024), (24 * 1024), (32 * 1024), (64 * 1024 - 128)},
	{(80 * 1024), (32 * 1024), (40 * 1024), (80 * 1024 - 128)},
};

struct efie {
	uint32_t	offset;
	uint32_t	length;
	uint8_t		checksum[16];
	uint8_t		is_checked;
	uint8_t		type;
	uint8_t		padding[102];
} __attribute__((packed));

static struct {
	char *name;
	int id[16];
	int filesize;
} firmware_table[] = {
	{"EVB",		{EVB, SC5, -1},				FILE_SIZE_64KB},
	{"SA5",		{SA5, SE5, SM5P, SM5S, -1},		FILE_SIZE_64KB},
	{"SC5PLUS",	{SC5PLUS, -1},				FILE_SIZE_64KB},
	{"SC5H",	{SC5H, -1},				FILE_SIZE_64KB},
	{"SC5PRO",	{SC5PRO, -1},				FILE_SIZE_64KB},
	{"SM5MINI",	{SM5ME, SM5MP, SM5MS, SM5MA, -1},	FILE_SIZE_64KB},
	{"SG2042EVB",	{SG2042EVB, MILKV_PIONEER, -1},	        FILE_SIZE_64KB},
	{"SG2042X4",	{SG2042X4, -1},				FILE_SIZE_80KB},
  	{"SM7G",        {SM7G, -1},	         		FILE_SIZE_64KB},
	{"SM7M_MP_1_2", {SM7M_MP_1_2, -1},	        	FILE_SIZE_64KB},
	{"ATHENA2EVB",	{ATHENA2EVB, -1},			FILE_SIZE_64KB},
	{"ATHENA2ACP",	{ATHENA2ACP, -1},			FILE_SIZE_64KB},
};

static const char *mcu_family_table[] = {"STM32L0", "GD32E50"};

struct fwinfo {
	uint8_t		md5[16];
	uint8_t		magic[4];
	uint8_t		type;
	uint8_t		mcu_family;
	uint8_t		r0[2];
	uint32_t	timestamp;
	uint8_t		r1[100];
} __attribute__((packed));

#define DUMP_COLUMN	16

static void dump(void *data, unsigned long len)
{
	unsigned long i;

	for (i = 0; i < len; ++i) {
		if (i % DUMP_COLUMN == 0)
			printf("%04lx: ", i);
		if (i % DUMP_COLUMN == DUMP_COLUMN - 1)
			printf("%02x\n", ((uint8_t *)data)[i]);
		else
			printf("%02x ", ((uint8_t *)data)[i]);
	}
	printf("\n");
}

static int i2c_send_u32(int fd, __u8 reg, __u32 val)
{
	__u8 buf[4];
	int err;

	buf[0] = (val >> 24) & 0xff;
	buf[1] = (val >> 16) & 0xff;
	buf[2] = (val >> 8) & 0xff;
	buf[3] = val & 0xff;
	err = i2c_smbus_write_i2c_block_data(fd, reg, sizeof(buf), buf);
	if (err < 0) {
		fprintf(stderr, "send u32 0x%08x to reg 0x%02x failed, error code %d\n",
			val, reg, err);
		return err;
	}
	return 0;
}

/* smbus read or write length is limited below 32 bytes, with this function, one
 * can send any length to device
 */
static int i2c_send_block(int fd, __u8 reg, void *data, int len)
{
	int smbus_block_max = 32;
	int slen, err, off, left;

	off = 0;

	while (off < len) {

		left = len - off;

		if (left >= smbus_block_max)
			slen = smbus_block_max;
		else
			slen = left;

		err = i2c_smbus_write_i2c_block_data(fd, reg, slen,
				&((__u8 *)data)[off]);
		if (err < 0) {
			fprintf(stderr, "send block failed\n");
			return err;
		}

		off += slen;
		reg += slen;
	}

	return 0;
}

static int i2c_recv_block(int fd, __u8 reg, void *data, int len)
{
	int smbus_block_max = 32;
	int slen, err, off, left;

	off = 0;

	while (off < len) {

		left = len - off;

		if (left >= smbus_block_max)
			slen = smbus_block_max;
		else
			slen = left;

		err = i2c_smbus_read_i2c_block_data(fd, reg, slen,
				&((__u8 *)data)[off]);
		if (err < 0) {
			fprintf(stderr, "send block failed\n");
			return err;
		}

		off += slen;
		reg += slen;
	}
	return 0;
}

static int flash_read(int i2c, void *data, unsigned int off, unsigned int len)
{
	/* send address */
	int flash_data_max = 128;
	int slen, err, xoff, left;

	xoff = 0;

	while (xoff < len) {

		left = len - xoff;

		if (left >= flash_data_max)
			slen = flash_data_max;
		else
			slen = left;

		err = i2c_send_u32(i2c, REG_FLASH_OFFSET, off + xoff);
		if (err)
			return err;

		err = i2c_recv_block(i2c, REG_FLASH_DATA,
				     &((__u8 *)data)[xoff], slen);
		if (err)
			return err;

		printf("read flash %08x %d%%\r",
		       off + xoff, xoff * 100 / len);

		xoff += slen;
	}

	printf("read flash %08x 100%%\n", off + xoff);

	return 0;
}

static int mcu_read(int i2c, unsigned int off, unsigned int len)
{
	void *data;
	int err;

	data = malloc(len);

	if (!data) {
		fprintf(stderr, "allocate memory failed\n");
		return ENOMEM;
	}

	if (flash_read(i2c, data, off, len)) {
		fprintf(stderr, "read flash data from mcu failed\n");
		err = EIO;
		goto out;
	}

	dump(data, len);

	err = 0;

out:
	free(data);
	return err;
}

static int flash_unlock(int i2c)
{
	int err;

	err = i2c_smbus_write_byte_data(i2c, REG_FLASH_CMD, FLASH_CMD_UNLOCK);

	if (err < 0) {
		fprintf(stderr, "unlock flash failed\n");
		return err;
	}

	return 0;
}

static int flash_lock(int i2c)
{
	int err;

	err = i2c_smbus_write_byte_data(i2c, REG_FLASH_CMD, FLASH_CMD_LOCK);

	if (err < 0) {
		fprintf(stderr, "lock flash failed\n");
		return err;
	}

	return 0;

}

static int flash_erase_page(int i2c, unsigned int off)
{
	int err;

	if (off & FLASH_PAGE_MASK) {
		fprintf(stderr, "offset should page aligned when erase\n");
		return -EINVAL;
	}

	err = i2c_send_u32(i2c, REG_FLASH_OFFSET, off);

	if (err < 0) {
		fprintf(stderr, "send flash offset failed\n");
		return -EIO;
	}

	err = i2c_smbus_write_byte_data(i2c, REG_FLASH_CMD, FLASH_CMD_ERASE);

	if (err < 0) {
		fprintf(stderr, "send erase command failed\n");
		return -EIO;
	}

	return 0;
}

static int flash_erase(int i2c, unsigned int off, unsigned int len)
{
	int i, err;

	if (off & FLASH_PAGE_MASK) {
		fprintf(stderr, "offset should page aligned when erase\n");
		return -EINVAL;
	}

	if (len & FLASH_PAGE_MASK) {
		fprintf(stderr, "length should page aligned when erase\n");
		return -EINVAL;
	}

	for (i = 0; i < len; i += FLASH_PAGE_SIZE) {
		printf("erase page %08x\r", off + i);
		err = flash_erase_page(i2c, off + i);
		if (err)
			return err;
	}
	printf("\n");

	return 0;
}

static int mcu_erase(int i2c, unsigned int off, unsigned int len)
{
	int err;

	err = flash_unlock(i2c);
	if (err)
		return err;

	err = flash_erase(i2c, off, len);
	if (err) {
		flash_lock(i2c);
		return err;
	}

	err = flash_lock(i2c);
	if (err)
		return err;

	return 0;
}

int flash_write(int i2c, void *data, unsigned int off, unsigned int len)
{
	/* send address */
	const int flash_data_max = 128;
	int slen, err, xoff, left, i, pgm;
	__u8 flash_data_buf[flash_data_max];
	void *read_back;

	xoff = 0;

	while (xoff < len) {

		left = len - xoff;

		if (left >= flash_data_max)
			slen = flash_data_max;
		else
			slen = left;

		memset(flash_data_buf, 0xff, sizeof(flash_data_buf));
		memcpy(flash_data_buf, &((__u8 *)data)[xoff], slen);

		/* if program is needed */
		pgm = false;
		for (i = 0; i < flash_data_max; ++i) {
			if (flash_data_buf[i] != 0xff) {
				pgm = true;
				break;
			}
		}

		if (pgm) {
			err = i2c_send_u32(i2c, REG_FLASH_OFFSET, off + xoff);
			if (err)
				return err;

			err = i2c_send_block(i2c, REG_FLASH_DATA,
					flash_data_buf, flash_data_max);
			if (err)
				return err;
		}

		printf("program flash %08x %d%%\r",
		       off + xoff, xoff * 100 / len);

		xoff += slen;
	}
	printf("program flash %08x 100%%\n", off + xoff);

	read_back = malloc(len);
	if (!read_back) {
		fprintf(stderr, "allocate read back buffer failed\n");
		return -ENOMEM;
	}

	err = flash_read(i2c, read_back, off, len);
	if (err) {
		fprintf(stderr, "read data from flash failed\n");
		goto free_read_back;
	}

	if (memcmp(read_back, data, len)) {
		err = -EIO;
		fprintf(stderr, "verify flash data failed\n");
		goto free_read_back;
	}

	printf("verify flash data ok\n");

free_read_back:
	free(read_back);
	return err;
}

static int mcu_write(int i2c, unsigned int off, const char *file)
{
	int fd, err;
	size_t file_size;
	void *data;
	struct stat stat;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "cannot open file %s\n", file);
		return -1;
	}

	err = fstat(fd, &stat);
	if (err < 0) {
		fprintf(stderr, "cannot stat file %s\n", file);
		goto close_file;
	}

	file_size = stat.st_size;

	data = malloc(file_size);
	if (!data) {
		fprintf(stderr, "cannot allocate memory for file %s\n",
			file);
		err = -ENOMEM;
		goto close_file;
	}

	if (read(fd, data, file_size) != file_size) {
		fprintf(stderr, "load file failed\n");
		err = -EIO;
		goto free_file;
	}

	err = flash_unlock(i2c);
	if (err)
		goto free_file;

	err = flash_write(i2c, data, off, file_size);
	if (err) {
		flash_lock(i2c);
		fprintf(stderr, "write file to flash failed\n");
		goto free_file;
	}

	err = flash_lock(i2c);
	if (err)
		goto free_file;

	err = 0;

free_file:
	free(data);
close_file:
	close(fd);
	return err;
}

static int check_firmware(int i2c, const char *file)
{
	int fd, err;
	size_t file_size;
	void *data;
	struct stat stat;

	struct fwinfo *fwinfo;
	int i, j, firmware_type;
	int board_type, mcu_family;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "cannot open file %s\n", file);
		return -1;
	}

	err = fstat(fd, &stat);
	if (err < 0) {
		fprintf(stderr, "cannot stat file %s\n", file);
		goto close_file;
	}

	file_size = stat.st_size;

	data = malloc(file_size);
	if (!data) {
		fprintf(stderr, "cannot allocate memory for file %s\n",
			file);
		err = -ENOMEM;
		goto close_file;
	}

	if (read(fd, data, file_size) != file_size) {
		fprintf(stderr, "load file failed\n");
		err = -EIO;
		goto free_file;
	}

	fwinfo = (void *)(((char *)data) + FWINFO_START(file_size));

	/* check magic */
	if (memcmp(fwinfo->magic, "MCUF", sizeof(fwinfo->magic))) {
		fprintf(stderr, "magic number not found\n");
		err = -EINVAL;
		goto free_file;
	}

	/* check md5 sum */
	MD5_CTX md_ctx;
	MD5Init(&md_ctx);
	unsigned long md_size = file_size - 128;
	void *expected_dgst = data + md_size;
	unsigned char calculated_dgst[16];
	MD5Update(&md_ctx, data, md_size);
	MD5Final(calculated_dgst, &md_ctx);
	if (memcmp(expected_dgst, calculated_dgst, 16)) {
		fprintf(stderr, "md5 check failed\n");
		fprintf(stderr,
			"maybe not a valid upgrade file or being damaged\n");
		goto free_file;
	}

	/* try to get mcu family */
	err = i2c_smbus_read_byte_data(i2c, REG_MCU_FAMILY);
	if (err < 0) {
		fprintf(stderr, "i2c bus cannot access\n");
		goto free_file;
	}
	if (err == 0 || err == 0xff)
		mcu_family = 0;
	else
		mcu_family = err;

	if (mcu_family >= ARRAY_SIZE(mcu_family_table)) {
		fprintf(stderr, "unknown target mcu family id %d\n",
			mcu_family);
		err = -EINVAL;
		goto free_file;
	}

	if (fwinfo->mcu_family >= ARRAY_SIZE(mcu_family_table)) {
		fprintf(stderr, "unknown firmware mcu family id %d\n",
			mcu_family);
		err = -EINVAL;
		goto free_file;
	}

	if (mcu_family != fwinfo->mcu_family) {
		fprintf(stderr, "mcu family not match\n");
		fprintf(stderr, "target mcu family %s\n",
			mcu_family_table[mcu_family]);
		fprintf(stderr, "firmware mcu family %s\n",
			mcu_family_table[fwinfo->mcu_family]);
		err = -EINVAL;
		goto free_file;
	}

	/* try to get board type */
	err = i2c_smbus_read_byte_data(i2c, REG_BOARD_TYPE);
	if (err < 0) {
		fprintf(stderr, "i2c bus cannot access\n");
		goto free_file;
	}
	board_type = err;

	firmware_type = fwinfo->type;

	for (i = 0; i < ARRAY_SIZE(firmware_table); ++i) {
		if (firmware_table[i].id[0] == firmware_type)
			break;
	}

	if (i == ARRAY_SIZE(firmware_table)) {
		/* unknown firmware type? strange */
		fprintf(stderr, "firmware not supported by this util\n");
		err = -EINVAL;
		goto free_file;
	}

	err = -EINVAL;
	for (j = 0; firmware_table[i].id[j] >= 0; ++j) {
		if (firmware_table[i].id[j] == board_type) {
			file_type = firmware_table[i].filesize;
			err = 0;
			break;
		}
	}

	if (err)
		fprintf(stderr, "firmware and board not match\n");

	err = 0;
free_file:
	free(data);
close_file:
	close(fd);
	return err;
}

static int mcu_upgrade_full(int i2c, const char *file)
{
	int err;
	struct stat statbuf;

	err = check_firmware(i2c, file);
	if (err)
		return err;

	err = stat(file, &statbuf);
	if (err) {
		perror("access file failed:");
		return err;
	}

	err = mcu_erase(i2c, 0, ROUND_UP(statbuf.st_size, FLASH_PAGE_SIZE));
	if (err)
		return err;

	return mcu_write(i2c, 0, file);
}

static int mcu_upgrade(int i2c, const char *file)
{
	int err, fd, i;
	struct efie *efie;
	struct stat stat;
	size_t file_size;
	void *data;

	err = check_firmware(i2c, file);
	if (err)
		return err;

	/* get application size */
	fd = open(file, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "cannot open file %s\n", file);
		return -1;
	}

	err = fstat(fd, &stat);
	if (err < 0) {
		fprintf(stderr, "cannot stat file %s\n", file);
		goto close_file;
	}

	file_size = stat.st_size;

	data = malloc(file_size);
	if (!data) {
		fprintf(stderr, "cannot allocate memory for file %s\n",
			file);
		err = -ENOMEM;
		goto close_file;
	}

	if (read(fd, data, file_size) != file_size) {
		fprintf(stderr, "load file failed\n");
		err = -EIO;
		goto free_file;
	}

	efie = (void *)((__u8 *)data + file_info[file_type].elif_start);
	for (i = 0; i < 32; ++i) {
		if (efie->type == EFIE_TYPE_APP)
			break;
	}

	if (efie->type != EFIE_TYPE_APP) {
		fprintf(stderr, "wrong firmware format\n");
		fprintf(stderr, "application efie not found\n");
		err = -EINVAL;
		goto free_file;
	}

	if (efie->offset !=  file_info[file_type].app_start) {
		fprintf(stderr, "delcared offset and real offset not match\n");
		err = -EINVAL;
		goto free_file;
	}

	err = flash_unlock(i2c);
	if (err)
		goto free_file;

	err = flash_erase(i2c, ROUND_DOWN(FWINFO_START(file_size), FLASH_PAGE_SIZE),
			FLASH_PAGE_SIZE);
	if (err) {
		fprintf(stderr, "erase mcu firmware infomation block failed\n");
		goto free_file;
	}

	err = flash_erase(i2c, file_info[file_type].app_start,
			  ROUND_UP(efie->length, FLASH_PAGE_SIZE));
	if (err) {
		fprintf(stderr, "erase flash failed\n");
		goto free_file;
	}

	err = flash_write(i2c, (__u8 *)data + file_info[file_type].app_start,
			  file_info[file_type].app_start, efie->length);

	if (err) {
		fprintf(stderr, "program flash failed\n");
		goto free_file;
	}

	err = flash_write(i2c, (__u8 *)data + FWINFO_START(file_size),
			FWINFO_START(file_size), sizeof(struct fwinfo));
	if (err) {
		fprintf(stderr, "program mcu firmware information block failed\n");
		goto free_file;
	}

	err = flash_lock(i2c);
	if (err)
		goto free_file;

free_file:
	free(data);
close_file:
	close(fd);

	return err;
}

static int i2c_bus_init(int bus, int addr)
{
	int fd;
	char dev_name[16];

	snprintf(dev_name, sizeof(dev_name), "/dev/i2c-%d", bus);
	fd = open(dev_name, O_RDONLY);
	if (fd < 0) {
		printf("open i2c device failed\n");
		return -1;
	}
	int err;
	err = ioctl(fd, I2C_SLAVE_FORCE, addr);
	if (err < 0) {
		printf("failed to set i2c slave address\n");
		close(fd);
		return -1;
	}

	return fd;
}

static void i2c_bus_destroy(int fd)
{
	close(fd);
}

static const char *usage_str[] = {
	"upgrade bus addr file",
	"upgrade-full bus addr file",
	"read bus addr offset length",
	"write bus addr offset file",
	"erase bus addr offset length",
};

void gd32e50_usage(void)
{
	int i;
	for (i = 0; i < sizeof(usage_str)/sizeof(usage_str[0]); ++i)
		printf("\tmcu-util %s\n", usage_str[i]);
}

static void usage(void)
{
	gd32e50_usage();
}

int gd32e50_main(int argc, char *argv[])
{
	int err = EINVAL;
	int i2c, bus, addr;
	unsigned long offset, length;

	bus = strtol(argv[2], NULL, 0);
	addr = strtol(argv[3], NULL, 0);

	i2c = i2c_bus_init(bus, addr);
	if (i2c < 0)
		return 1;

	if (strcmp(argv[1], "read") == 0 && argc == 6) {
		offset = strtoul(argv[4], NULL, 0);
		length = strtoul(argv[5], NULL, 0);
		err = mcu_read(i2c, offset, length);
	} else if (strcmp(argv[1], "write") == 0 && argc == 6) {
		offset = strtoul(argv[4], NULL, 0);
		err = mcu_write(i2c, offset, argv[5]);
	} else if (strcmp(argv[1], "upgrade-full") == 0 && argc == 5) {
		err = mcu_upgrade_full(i2c, argv[4]);
	} else if (strcmp(argv[1], "upgrade") == 0 && argc == 5) {
		err = mcu_upgrade(i2c, argv[4]);
	} else if (strcmp(argv[1], "erase") == 0 && argc == 6) {
		offset = strtoul(argv[4], NULL, 0);
		length = strtoul(argv[5], NULL, 0);
		err = mcu_erase(i2c, offset, length);
	}

	i2c_bus_destroy(i2c);

	if (err) {
		usage();
		err = err < 0 ? -err : err;
	}


	return err;
}
