#include <sd_io.h>
#include <debug.h>
#include <string.h>
#include <tick.h>

static SD_DEV sd;

int sd_init(void)
{
	int err = SD_Init(&sd);
	return  err == SD_OK ? 0 : -1;
}

int sd_read(void *data, unsigned long sector, unsigned long num)
{
	unsigned long i;

	for (i = 0; i < num; ++i) {
		int err = SD_Read(&sd, (uint8_t *)data + i * 512, sector, 0, 512);
		if (err != SD_OK)
			return -1;
	}

	return 0;
}

int sd_write(void *data, unsigned long sector, unsigned long num)
{
	unsigned long i;

	for (i = 0; i < num; ++i, ++sector) {
		int err = SD_Write(&sd, (uint8_t *)data + i * 512, sector);
		if (err != SD_OK) {
			return -1;
		}
	}

	return 0;
}

int sd_test(void)
{
	const unsigned long sector = 1;
	uint8_t rd[512];
	uint8_t wr[512];
	memset(rd, 0xff, sizeof(rd));
	memset(wr, 0x5a, sizeof(wr));

	if (sd_read(rd, sector, 1)) {
		debug("card read failed\r\n");
		return -1;
	}

	debug("original sector %lu data:\r\n", sector);
	dump(rd, sizeof(rd));

	if (sd_write(wr, sector, 1)) {
		debug("card write failed\r\n");
		return -1;
	}

	debug("rewrite sector %lu with 0x5a\r\n", sector);

	if (sd_read(rd, sector, 1)) {
		debug("card read failed\r\n");
		return -1;
	}
	dump(rd, sizeof(rd));

	if (memcmp(rd, wr, sizeof(rd))) {
		debug("data write error\r\n");
		return -1;
	}

	debug("seems good\r\n");
	return 0;
}

int sd_benchmark(void)
{
	uint8_t buf[512];
	unsigned long num = 1024 * 1024;
	unsigned long start;
	unsigned long i;

	memset(buf, 0xff, sizeof(buf));

	start = tick_get();

	for (i = 0; i < num; ++i) {
		sd_write(buf, i, 1);
		if (i % 32 == 31)
			error("%ld sectors used %ld ms\r",
			      i, (long)(tick_get() - start));
	}
	error("\r\n");
	return 0;
}
