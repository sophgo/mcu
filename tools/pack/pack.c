/*
 * pack all thing to a image
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <global.h>
#include <md5.h>

#define FLASH_SIZE		(64 * 1024)
#define PROGRAM_LIMIT		(FLASH_SIZE - 128)

#define LOADER_START		0
#define LOADER_SIZE		(28 * 1024)
#define LOADER_END		(LOADER_START + LOADER_SIZE)

#define EFIT_START		LOADER_END
#define EFIT_SIZE		(4 * 1024)
#define EFIT_END		(EFIT_START + EFIT_SIZE)
#define EFIE_SIZE		(128)

struct efie {
	uint32_t	offset;
	uint32_t	length;
	uint8_t		checksum[16];
	uint8_t		reserved[104];
} __attribute__((packed));

struct comp {
	const char *file;
	unsigned long size;
	unsigned char *buf;
	struct efie efie;
};

void checksum(void *out, void *in, unsigned long len)
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

int load_file(struct comp *comp, const char *file)
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

int store_file(void *buf, unsigned long size, const char *file)
{
	int fd;
	int err = -1;

	fd = open(file, O_RDWR | O_CREAT,
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if (fd < 0) {
		fprintf(stderr, "cannot open file %s\n", file);
		perror("");
		return -1;
	}

	if (write(fd, buf, size) != size) {
		fprintf(stderr, "cannot store hole file %s\n", file);
		err = -1;
		goto close_file;
	}
	err = 0;
close_file:
	close(fd);
	return err;
}

void print_efie(struct efie *efie)
{
	printf("offset 0x%08x length 0x%08x checksum ",
	       efie->offset, efie->length);
	int i;
	for (i = 0; i < 16; ++i)
		printf("%02x", efie->checksum[i]);
	printf("\n");
}

void usage(void)
{
	printf("usage: pack loader application application-offset upgrader upgrader-offset flash-image-file\n");
}

int is_invalid(struct comp *loader, struct comp *app, struct comp *upgrader)
{
	// check offset
	if (app->efie.offset < EFIT_END) {
		fprintf(stderr, "application start offset should over efit end\n");
		return -1;
	}

	if (upgrader->efie.offset < EFIT_END) {
		fprintf(stderr, "upgrader start offset should over efit end\n");
		return -1;
	}

	if (loader->efie.length >= EFIT_START) {
		fprintf(stderr, "loader size too big\n");
		return -1;
	}

	unsigned long region0_start = app->efie.offset;
	unsigned long region0_end = app->efie.offset + app->efie.length;
	unsigned long region1_start = upgrader->efie.offset;
	unsigned long region1_end = upgrader->efie.offset +
					upgrader->efie.length;

	if (!(region0_end <= region1_start || region0_start >= region1_end)) {
		fprintf(stderr, "application and upgrader overlay\n");
		return -1;
	}

	if (region0_end > PROGRAM_LIMIT) {
		fprintf(stderr, "application out of valid program region\n");
		return -1;
	}
	if (region1_end > PROGRAM_LIMIT) {
		fprintf(stderr, "upgrader out of valid program region\n");
		fprintf(stderr, "end of upgrader 0x%08lx\n", region1_end);
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 7) {
		usage();
		return 1;
	}
	assert(sizeof(struct efie) == 128);
	struct comp app, loader, upgrader;
	unsigned char *image;
	image = malloc(FLASH_SIZE);
	if (image == NULL) {
		fprintf(stderr, "cannot alloc memory for image\n");
		return 1;
	}
	memset(image, 0x00, FLASH_SIZE);
	if (load_file(&loader, argv[1]))
		return 1;
	if (load_file(&app, argv[2]))
		return 1;
	if (load_file(&upgrader, argv[4]))
		return 1;

	loader.efie.offset = 0;
	app.efie.offset = strtol(argv[3], NULL, 0);
	upgrader.efie.offset = strtol(argv[5], NULL, 0);

	print_efie(&loader.efie);
	print_efie(&app.efie);
	print_efie(&upgrader.efie);

	if (is_invalid(&loader, &app, &upgrader))
		return 1;

	memcpy(image + loader.efie.offset, loader.buf, loader.size);
	memcpy(image + app.efie.offset, app.buf, app.size);
	memcpy(image + upgrader.efie.offset, upgrader.buf, upgrader.size);

	memcpy(image + EFIT_START, &app.efie, EFIE_SIZE);
	memcpy(image + EFIT_START + EFIE_SIZE, &upgrader.efie, EFIE_SIZE);

	/* calculate md5sum, from 0 to 64K - 16 */
	MD5_CTX md_ctx;
	MD5Init(&md_ctx);
	unsigned long md_size = PROGRAM_LIMIT;
	void *dgst = image + md_size;
	MD5Update(&md_ctx, image, md_size);
	MD5Final(dgst, &md_ctx);

	store_file(image, FLASH_SIZE, argv[6]);

	return 0;
}

