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

#include <global.h>
#include <md5.h>
#include <i2c.h>
#include <project.h>

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

#define FLASH_SIZE		(64 * 1024)
#define PROGRAM_LIMIT		(FLASH_SIZE - 128)

#define LOADER_START		0
#define LOADER_SIZE		(28 * 1024)
#define LOADER_END		(LOADER_START + LOADER_SIZE)

#define EFIT_START		LOADER_END
#define EFIT_SIZE		(4 * 1024)
#define EFIT_END		(EFIT_START + EFIT_SIZE)
#define EFIE_SIZE		(128)

#if 0
#define APP_START		EFIT_END
#define APP_SIZE		(28 * 1024)
#define APP_END			(APP_START + APP_SIZE)

#define UPGRADER_START		APP_END
#define UPGRADER_SIZE		(4 * 1024)
#define UPGRADER_END		(UPGRADER_START + UPGRADER_SIZE)
#endif

#define REG_BOARD_TYPE		(0x00)
#define REG_STAGE		(0x3c)
#define REG_LOG			(0x62)
#define REG_CMD			(0x03)
#define REG_CALC_CKSUM		(0x63)
#define REG_CKSUM_OFF		(0x64)
#define REG_CKSUM_LEN		(0x68)
#define REG_CKSUM		(0x6c)
#define REG_OFFSET		(0x7c)
#define REG_DATA		(0x80)
#define REG_FLUSH		(0xff)

#define CMD_MCU_UPDATE		(0x08)

#define STAGE_APP		0
#define STAGE_LOADER		1
#define STAGE_UPGRADER		2

struct efie {
	__u32	offset;
	__u32	length;
	__u8	checksum[16];
	__u8	reserved[104];
} __attribute__((packed));

struct comp {
	const char *file;
	unsigned long size;
	unsigned char *buf;
	struct efie efie;
};

static struct {
	char *name;
	int id[16];
} firmware_table[] = {
	{"EVB",         {EVB, SC5, -1}},
	{"SA5",         {SA5, SE5, SM5P, SM5S, -1}},
	{"SC5PLUS",     {SC5PLUS, -1}},
	{"SC5H",        {SC5H, -1}},
	{"SC5PRO",      {SC5PRO, -1}},
	{"SM5MINI",     {SM5ME, SM5MP, SM5MS, SM5MA, SM5SE6M, -1} },
	{"SE5LITE",     {SE5LITE, -1}},
	{"BM1684XEVB",  {BM1684XEVB, BM1684XEVB, -1}},
	{"SM7CUSTV1",   {SM7CUSTV1, SM7CUSTV1, -1}},
	{"SM7MINI",     {SM7M, -1}},
	{"SM7MQY",      {SM7MQY, SE7Q, -1}},
	{"SE7",         {SE7, -1}},
	{"SM7MSE6M",    {SM7MSE6M, -1}},
	{"SM7M_MP_1_1",		{SM7M_MP_1_1, -1}},
	{"SM7_HK",         {SM7_HK, -1}},
};

struct fwinfo {
	uint8_t		magic[4];
	uint8_t		type;
	uint8_t		r0[3];
	uint32_t	timestamp;
} __attribute__((packed));


static int fd;

static void hexdump_string(void *data, unsigned long len)
{
	int i;
	for (i = 0; i < len; ++i)
		printf("%02x", ((unsigned char *)data)[i]);
}

static void checksum(void *out, void *in, unsigned long len)
{
	uint32_t *src = in;
	char *init = "*BITMAIN-SOPHON*";
	uint32_t result[4];
	memcpy(result, init, sizeof(result));
	unsigned long block = len >> 4;
	unsigned long left = len & 0xf;
	unsigned long i, j;
	for (i = 0; i < block; ++i, src += 4) {
		for (j = 0; j < 4; ++j) {
			result[j] ^= src[j];
		}
	}
	for (i = 0; i < left; ++i) {
		((uint8_t *)result)[i] ^= ((uint8_t *)src)[i];
	}
	memcpy(out, result, sizeof(result));
}

static int i2c_read_i2c_block(int fd, unsigned int reg,
			      void *data, unsigned int len)
{
	int err;
	int i;
	for (i = 0; i < len; ++i) {
		err = i2c_smbus_read_byte_data(fd, i + reg);
		if (err < 0) {
			fprintf(stderr, "read data failed\n");
			return -1;
		}
		((unsigned char *)data)[i] = err;
	}
	return 0;
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
		perror("");
		return -1;
	}
	return 0;
}

static int i2c_send_page(int fd, void *data, unsigned long offset)
{
	int block, err;
	if (i2c_send_u32(fd, REG_OFFSET, offset))
		return -1;

	for (block = 0; block < 128 / 32; ++block){
		err = i2c_smbus_write_i2c_block_data(fd,
						     REG_DATA + block * 32,
						     32,
						     (__u8 *)data + block * 32);
		if (err < 0) {
			fprintf(stderr, "send smbus block failed\n");
			fprintf(stderr, "%dth block data\n", block);
			return -1;
		}
	}

	unsigned int times = 0;
	do {
		err = i2c_smbus_read_byte_data(fd, 0);
		++times;
		if (times > 200) {
			fprintf(stderr, "try %d times but still not responding\n", times);
			return -1;
		}

		if (times < 100)
			usleep(1000);
		else
			usleep(5 * 1000);

	} while (err < 0);
	return 0;
}

static int i2c_get_checksum(int fd, unsigned long offset,
			    unsigned long length, void *cksum)
{
	unsigned char calc_cksum[16];
	// start offset
	if (i2c_send_u32(fd, REG_CKSUM_OFF, offset)) {
		return -1;
	}
	// calculated length
	if (i2c_send_u32(fd, REG_CKSUM_LEN, length)) {
		return -1;
	}
	// start calculation
	int err;
	err = i2c_smbus_write_byte_data(fd, REG_CALC_CKSUM, 1);
	if (err) {
		fprintf(stderr, "cannot require checksum calculation\n");
		return -1;
	}
	do {
		usleep(50 * 1000);
		err = i2c_smbus_read_byte_data(fd, REG_CALC_CKSUM);
		if (err < 0) {
			fprintf(stderr, "cannot check checksum status\n");
			return -1;
		}
	} while (err);

	memset(calc_cksum, 0x00, sizeof(calc_cksum));
	if (i2c_read_i2c_block(fd, REG_CKSUM, calc_cksum, 16)) {
		return -1;
	}
	memcpy(cksum, calc_cksum, 16);
	return 0;
}

static int i2c_program_flash(int fd, void *data, unsigned long size,
			     unsigned long offset, int verify)
{
	if (offset & (128 - 1)) {
		fprintf(stderr, "program offset must 128 byte aligned\n");
		return -1;
	}

	printf("program flash offset 0x%08lx length 0x%08lx\n", offset, size);

	unsigned long nbytes;
	__u8 tmp[128];
	for (nbytes = 0; nbytes < size; nbytes += 128) {
		memset(tmp, 0x00, 128);
		unsigned long burn_size =
			size - nbytes >= 128 ? 128 : size - nbytes;
		memcpy(tmp, ((__u8*)data) + nbytes, burn_size);
		if (i2c_send_page(fd, tmp, offset + nbytes)) {
			printf("\n");
			return -1;
		}
		unsigned long percentage = (nbytes * 100 / size);
		percentage = percentage > 100 ? 100 : percentage;
		printf("program page, offset 0x%08lx, %ld%%\r",
		       offset + nbytes, percentage);
		fflush(stdout);
	}
	printf("program page, offset 0x%08lx, %ld%%\r\n",
	       offset + nbytes - 128, 100UL);

	if (verify) {
		unsigned char calc_cksum[16];
		unsigned char expected_cksum[16];
		// calculate checksum
		memset(calc_cksum, 0x00, sizeof(calc_cksum));
		if (i2c_get_checksum(fd, offset, size, calc_cksum)) {
			fprintf(stderr, "require checksum failed\n");
			return -1;
		}
		checksum(expected_cksum, data, size);
		if (memcmp(calc_cksum, expected_cksum, 16)) {
			fprintf(stderr, "checksum error\n");
			printf("expected checksum ");
			hexdump_string(expected_cksum, 16);
			printf("\n");
			printf("calculated checksum ");
			hexdump_string(calc_cksum, 16);
			printf("\n");
			return -1;
		}
	}

	return 0;
}

static int i2c_read_flash(int fd, void *data,
			  unsigned long size, unsigned long offset)
{
	if (offset & (128 - 1)) {
		fprintf(stderr, "program offset must 128 byte aligned\n");
		return -1;
	}

	unsigned long i, j;

	for (i = offset, j = 0; j < size; ++i, ++j) {
		if (i % 128 == 0)
			if (i2c_send_u32(fd, REG_OFFSET, i))
				return -1;
		int err;
		err = i2c_smbus_read_byte_data(fd, REG_DATA + (i & 127));
		if (err < 0) {
			fprintf(stderr, "read data failed\n");
			return -1;
		}
		((uint8_t *)data)[j] = (uint8_t)err;
	}
	return 0;
}

static void print_efie(struct efie *efie)
{
	printf("offset 0x%08x length 0x%08x checksum ",
	       efie->offset, efie->length);
	int i;
	for (i = 0; i < 16; ++i)
		printf("%02x", efie->checksum[i]);
	printf("\n");
}


static void unload_file(struct comp *comp)
{
	if (comp->buf)
		free(comp->buf);
}

static int load_file(struct comp *comp, const char *file)
{
	int fd;
	int err = -1;
	struct stat stat;

	memset(comp, 0x00, sizeof(*comp));

	comp->file = file;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "cannot load file %s\n", file);
		return -1;
	}

	err = fstat(fd, &stat);
	if (err < 0) {
		fprintf(stderr, "cannot stat file %s\n", file);
		goto close_file;
	}
	comp->size = stat.st_size;
	comp->buf = malloc(comp->size);
	if (comp->buf == NULL) {
		fprintf(stderr, "cannot malloc buffer for file %s\n", file);
		err = -1;
		goto close_file;
	}
	if (read(fd, comp->buf, comp->size) != comp->size) {
		fprintf(stderr, "cannot load hole file %s\n", file);
		err = -1;
		goto close_file;
	}

	checksum(comp->efie.checksum, comp->buf, comp->size);
	comp->efie.length = comp->size;

	err = 0;
close_file:
	close(fd);
	return err;
}

static int enter_upgrader(void)
{
	int err;
	//check if we are in upgrader stage
	do {
		err = i2c_smbus_read_byte_data(fd, REG_STAGE);
		if (err < 0) {
			fprintf(stderr, "i2c bus cannot access\n");
			close(fd);
			return -1;
		}
		if (err != STAGE_UPGRADER) {
			// we are not in upgrader stage
			fprintf(stderr, "not in upgrader stage try to enter, stage %d\n", err);
			i2c_smbus_write_byte_data(fd, REG_CMD, CMD_MCU_UPDATE);
			sleep(1);
		} else
			break;
	} while (1);
	return 0;
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
	"checksum bus addr offset length",
	"version bus addr",
};

void stm32l0_usage(void)
{
	int i;
	for (i = 0; i < sizeof(usage_str)/sizeof(usage_str[0]); ++i)
		printf("\tmcu-util %s\n", usage_str[i]);
}

static void usage(void)
{
	stm32l0_usage();
}

static int version(void)
{

	int err;

	if (enter_upgrader())
		return -1;

	if (i2c_smbus_write_byte_data(fd, REG_LOG, 0)) {
		fprintf(stderr, "failed to reset log pointer\n");
		return -1;
	}

	printf("upgrader ");
	do {
		err = i2c_smbus_read_byte_data(fd, REG_LOG);
		if (err < 0) {
			fprintf(stderr, "failed to get version string\n");
			return -1;
		}
		printf("%c", (char)err);
	} while (err);
	printf("\n");
	return 0;
}

static int is_valid_firmware(void *image, unsigned long size)
{
	struct fwinfo *fwinfo;
	int i, j, firmware_type, err;
	int board_type;

	fwinfo = (void *)(((char *)image) + size - 128 + 16);

	/* check magic */
	if (memcmp(fwinfo->magic, "MCUF", sizeof(fwinfo->magic))) {
		/* lagecy firmware, that does not contain firmware type,
		 * let it go
		 */
		return true;
	}

	err = i2c_smbus_read_byte_data(fd, REG_STAGE);
	if (err < 0) {
		fprintf(stderr, "i2c bus cannot access\n");
		return false;
	}

	if (err == STAGE_UPGRADER) {
		/* user execute this command when mcu is working on upgrader
		 * stage, i2c upgrader does not support getting board type.
		 * let it go
		 */
		return true;
	}
	/* try to get board type */
	err = i2c_smbus_read_byte_data(fd, REG_BOARD_TYPE);
	if (err < 0) {
		fprintf(stderr,
			"i2c bus cannot access\n");
		close(fd);
		return false;
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
		return false;
	}

	for (j = 0; firmware_table[i].id[j] >= 0; ++j) {
		if (firmware_table[i].id[j] == board_type)
			return true;
	}

	fprintf(stderr, "firmware and board not match\n");
	return false;
}

static int is_invalid(void *_image, unsigned long size)
{
	unsigned char * const image = _image;

	/* check file size */
	if (size != FLASH_SIZE) {
		fprintf(stderr,
			"wrong upgrade file size %ld, it should 0x%ld bytes\n",
			size, (unsigned long)FLASH_SIZE);
		return -1;
	}

	/* check md5 sum */
	MD5_CTX md_ctx;
	MD5Init(&md_ctx);
	unsigned long md_size = PROGRAM_LIMIT;
	void *expected_dgst = image + md_size;
	unsigned char calculated_dgst[16];
	MD5Update(&md_ctx, image, md_size);
	MD5Final(calculated_dgst, &md_ctx);
	if (memcmp(expected_dgst, calculated_dgst, 16)) {
		fprintf(stderr, "md5 check failed\n");
		fprintf(stderr, "maybe not a valid upgrade file or being damaged\n");
		return -1;
	}

	/* check board type */
	if (!is_valid_firmware(_image, size)) {
		fprintf(stderr, "firmware and board not match\n");
		return -1;
	}

	/* check application efie */
	struct efie *app_efie;

	app_efie = (struct efie *)(image + EFIT_START);
	if (app_efie->offset + app_efie->length > PROGRAM_LIMIT) {
		fprintf(stderr, "wrong efie of app\n");
		return -1;
	}

	void *app_data = image + app_efie->offset;
	unsigned long app_len = app_efie->length;

	uint8_t calc_cksum[16];

	checksum(calc_cksum, app_data, app_len);

	if (memcmp(calc_cksum, app_efie->checksum, 16)) {
		fprintf(stderr, "checksum of app wrong\n");
		return -1;
	}
	return 0;
}

static int mcu_upgrade(char *file)
{
	int err;
	// load file from disk
	struct comp img = {0};

	if (load_file(&img, file))
		return -1;

	if (is_invalid(img.buf, img.size)) {
		err = -1;
		goto unload;
	}

	if (enter_upgrader()) {
		err = -1;
		goto unload;
	}

	struct efie *app_efie = (struct efie *)(img.buf + EFIT_START);
	print_efie(app_efie);
	void *app_data = img.buf + app_efie->offset;
	unsigned long app_len = app_efie->length;

	// program efie
	if (i2c_program_flash(fd, app_efie, sizeof(*app_efie), EFIT_START, 1)) {
		err = -1;
		goto unload;
	}
	// program app
	if (i2c_program_flash(fd, app_data, app_len, app_efie->offset, 1)) {
		err = -1;
		goto unload;
	}

	// calculate checksum
	uint8_t calc_cksum[16];
	memset(calc_cksum, 0x00, sizeof(calc_cksum));
	if (i2c_get_checksum(fd, app_efie->offset,
			     app_efie->length, calc_cksum)) {
		err = -1;
		goto unload;
	}
	if (memcmp(calc_cksum, app_efie->checksum, 16)) {
		err = -1;
		fprintf(stderr, "checksum error\n");
		printf("expected checksum ");
		hexdump_string(app_efie->checksum, 16);
		printf("\n");
		printf("calculated checksum ");
		hexdump_string(calc_cksum, 16);
		printf("\n");
		goto unload;
	}

	err = 0;
unload:
	unload_file(&img);
	return err;
}

static int mcu_upgrade_full(char *file)
{
	int err;
	// load file from disk
	struct comp img;

	if (load_file(&img, file))
		return 0;

/* workaround:support for upgrading to defferent board type*/
#if 0
	if (is_invalid(img.buf, img.size)) {
		err = -1;
		goto unload;
	}
#endif

	if (enter_upgrader()) {
		err = -1;
		goto unload;
	}

	// program image
	if (i2c_program_flash(fd, img.buf, img.size, 0, 1)) {
		err = -1;
		goto unload;
	}

	err = 0;
unload:
	unload_file(&img);
	return err;
}

static int mcu_cksum(unsigned long offset, unsigned long length)
{
	unsigned char calc_cksum[16];

	if (enter_upgrader())
		return -1;
	if (i2c_get_checksum(fd, offset, length, calc_cksum)) {
		return -1;
	}
	printf("offset 0x%08lx length 0x%08lx checksum ", offset, length);
	hexdump_string(calc_cksum, 16);
	printf("\n");

	return 0;
}

static int mcu_read(unsigned long offset, unsigned long length)
{
	int err;

	if (offset + length > 64 * 1024) {
		fprintf(stderr, "required region out of flash\n");
		return -1;
	}

	if (offset & 127) {
		fprintf(stderr, "offset must 128 bytes aligned\n");
		return -1;
	}

	uint8_t *data = malloc(64 * 1024);
	if (data == NULL) {
		fprintf(stderr, "cannot allocate memory\n");
		return -1;
	}

	if (enter_upgrader())
		return -1;

	unsigned long i, xoffset;

	for (i = 0, xoffset = offset; i < length; ++i, ++xoffset) {
		if (xoffset % 128 == 0) {
			if (i2c_read_flash(fd, data + i, 128, xoffset)) {
				err = -1;
				goto free_data;
			}
		}
		if (xoffset % 16 == 0)
			printf("\n%08lx:", xoffset);
		printf(" %02x", data[i]);
		fflush(stdout);
	}
	printf("\n");

	err = 0;
free_data:
	free(data);

	return err;
}

static int mcu_write(unsigned long offset, char *file)
{
	struct comp comp;
	int err;

	if (load_file(&comp, file))
		return -1;

	if (offset & 127) {
		fprintf(stderr, "offset must 128 bytes aligned\n");
		err = -1;
		goto unload;
	}

	if (enter_upgrader())
		return -1;

	if (i2c_program_flash(fd, comp.buf, comp.size, offset, 1)) {
		fprintf(stderr, "offset must 128 bytes aligned\n");
		err = -1;
		goto unload;
	}
unload:
	err = 0;
	unload_file(&comp);
	return err;
}

int stm32l0_main(int argc, char *argv[])
{
	if (argc < 5) {
		printf("invalid usage\n");
		usage();
		return 1;
	}

	int bus = strtol(argv[2], NULL, 0);
	int addr = strtol(argv[3], NULL, 0);

	int err;
	fd = i2c_bus_init(bus, addr);
	if (fd < 0)
		return 1;

	if (strcmp("upgrade", argv[1]) == 0) {
		if (argc != 5) {
			usage();
			err = 1;
			goto close_dev;
		}
		if (mcu_upgrade(argv[4])) {
			err = 1;
			goto close_dev;
		}
	} else if (strcmp("upgrade-full", argv[1]) == 0) {
		if (argc != 5) {
			usage();
			err = 1;
			goto close_dev;
		}
		if (mcu_upgrade_full(argv[4])) {
			err = 1;
			goto close_dev;
		}
	} else if (strcmp("read", argv[1]) == 0) {
		if (argc != 6) {
			usage();
			err = 1;
			goto close_dev;
		}
		unsigned long offset, length;
		offset = strtol(argv[4], NULL, 0);
		length = strtol(argv[5], NULL, 0);
		if (mcu_read(offset, length)) {
			err = 1;
			goto close_dev;
		}
	} else if (strcmp("write", argv[1]) == 0) {
		if (argc != 6) {
			usage();
			err = 1;
			goto close_dev;
		}
		unsigned long offset;
		offset = strtol(argv[4], NULL, 0);
		if (mcu_write(offset, argv[5])) {
			err = 1;
			goto close_dev;
		}
	} else if (strcmp("checksum", argv[1]) == 0) {
		unsigned long offset, length;
		offset = strtol(argv[4], NULL, 0);
		length = strtol(argv[5], NULL, 0);
		if (mcu_cksum(offset, length)) {
			err = 1;
			goto close_dev;
		}
	} else if (strcmp("version", argv[1]) == 0) {
		if (version()) {
			err = 1;
			goto close_dev;
		}
	} else {
		fprintf(stderr, "unknown command %s\n", argv[1]);
		usage();
		err = 1;
		goto close_dev;
	}

	err = 0;
close_dev:
	i2c_bus_destroy(fd);
	return err;
}

