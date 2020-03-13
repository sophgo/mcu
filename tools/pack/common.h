#ifndef __COMMON_H__
#define __COMMON_H__

enum {
	COMP_TYPE_APP = 0,
	COMP_TYPE_LOADER = 1,
	COMP_TYPE_I2C_UPGRADER = 2,
	COMP_TYPE_UART_UPGRADER = 3,
};

struct efie {
	uint32_t	offset;
	uint32_t	length;
	uint8_t		checksum[16];
	uint32_t	type;
	uint8_t		padding[100];
} __attribute__((packed));

struct comp {
	const char *file;
	unsigned long size;
	unsigned char *buf;
	struct efie efie;
};

void checksum(void *out, void *in, unsigned long len);
int load_file(struct comp *comp, const char *file);
void unload_file(struct comp *comp);
int store_file(void *buf, unsigned long size, const char *file);
void print_efie(struct efie *efie);

#endif
