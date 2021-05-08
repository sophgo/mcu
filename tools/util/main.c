#include <linux/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <i2c.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef VERSION
#define VERSION "unversioned"
#endif

#define REG_MCU_FAMILY	0x18

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

void stm32l0_usage(void);
int stm32l0_main(int argc, char *argv[]);
void gd32e50_usage(void);
int gd32e50_main(int argc, char *argv[]);

struct mcu_family {
	char *name;
	int id;
	void (*usage)(void);
	int (*main)(int argc, char *argv[]);
} mcu_family_table[] = {
	{"STM32L0", 0, stm32l0_usage, stm32l0_main},
	{"GD32E50", 1, gd32e50_usage, gd32e50_main},
};

static void usage(void)
{
	int i;

	printf("mcu-util %s\n\n", VERSION " " __DATE__ " " __TIME__);

	for (i = 0; i < ARRAY_SIZE(mcu_family_table); ++i) {
		printf("MCU Family %s\n", mcu_family_table[i].name);
		mcu_family_table[i].usage();
	}
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

static struct mcu_family *get_mcu_family(int bus, int addr)
{
	struct mcu_family *mcuf = NULL;
	int i2c, err, i;

	i2c = i2c_bus_init(bus, addr);

	if (i2c < 0)
		return NULL;

	err = i2c_smbus_read_byte_data(i2c, REG_MCU_FAMILY);
	if (err < 0) {
		fprintf(stderr, "i2c bus error\n");
		perror("error reason: ");
		goto out;
	}

	/* some old stm32 firmware may return 0xff on mcu family register */
	err = (err == 0xff ? 0 : err);

	for (i = 0; i < ARRAY_SIZE(mcu_family_table); ++i) {
		if (mcu_family_table[i].id == err) {
			mcuf = &mcu_family_table[i];
			break;
		}
	}

	if (!mcuf) {
		fprintf(stderr, "unknown mcu family, id %d\n", err);
		goto out;
	}

out:
	i2c_bus_destroy(i2c);
	return mcuf;

}

int main(int argc, char *argv[])
{
	struct mcu_family *mcuf;
	int bus, addr;

	if (argc < 4) {
		usage();
		return 1;
	}

	bus = strtol(argv[2], NULL, 0);
	addr = strtol(argv[3], NULL, 0);

	mcuf = get_mcu_family(bus, addr);

	if (!mcuf)
		return 1;

	if (strcmp(argv[1], "info") == 0) {
		printf("MCU Family %s\n", mcuf->name);
		return 0;
	}

	return mcuf->main(argc, argv);
}
